# sadly, python is super slow; this script takes a bit over a minute
# vs. a few seconds for the C version. but i can't figure out how to
# quickly serialize the C gaddag. ugh.

import sys
import logging
import struct
import copy
import json

logger = logging.getLogger(__name__)

h1 = logging.StreamHandler()
logger.addHandler(h1)
logger.setLevel(logging.INFO)


allocArcs = 0
allocStates = 0
nodes = []
arcs = []


class Node:
    def __init__(self):
        global allocStates
        global nodes
        self.letterSet = set()
        self.arcs = {}
        self.id = allocStates
        allocStates += 1
        nodes.append(self)

    def _contains_arc(self, c):
        return c in self.arcs

    def add_arc(self, c):
        # adds an arc from this node for c (it it doesn't exist)
        # and returns the node that this arc leads to
        #logger.debug('Called add_arc for state %s, char %s' % (self, c))
        if self._contains_arc(c):
            foundArc = self.arcs[c]
        else:
            foundArc = Arc(c, self)
            self.arcs[c] = foundArc

        return foundArc.destNode

    def add_final_arc(self, c1, c2):
        # add arc from this state to c1 and add c2 to this arc's letter set
        #logger.debug('(Called add_final_arc for state %s, chars %s %s)' % (
        #             self, c1, c2))
        node = self.add_arc(c1)
        assert(c2 not in node.letterSet)
        node.letterSet.add(c2)
        return node

    def force_arc(self, c, forceState):
        # add arc from this state to forceState for c
        #logger.debug('Called force_arc for state %s, char %s, endState %s' % (
        #    self, c, forceState))
        if self._contains_arc(c) and self.arcs[c].destNode is not forceState:
            assert(False)
        self.arcs[c] = Arc(c, self, forceState)

    def __str__(self):
        theseArcs = []
        for arc in self.arcs:
            theseArcs.append({
                self.arcs[arc].id: "%s (%s -> %s)" % (
                    self.arcs[arc].letter,
                    self.arcs[arc].fromNode.id,
                    self.arcs[arc].destNode.id)
            })
        obj = {'Node': self.id,
               'letterSet': [letter for letter in self.letterSet],
               'arcs': theseArcs}

        return json.dumps(obj, indent=2)


class Arc:
    def __init__(self, c, fromNode, toNode=None):
        global allocArcs
        global arcs
        self.letter = c
        if toNode is None:
            toNode = Node()
        self.destNode = toNode
        self.fromNode = fromNode
        self.id = allocArcs
        allocArcs += 1
        arcs.append(self)

    def __str__(self):
        obj = {'Arc': self.id,
               'letter': self.letter,
               'fromNode': self.fromNode.id,
               'destNode': self.destNode.id}
        return json.dumps(obj, indent=2)


def get_words(filename):
    words = []
    f = open(filename, 'rb')
    for line in f:
        words.append(line.split(' ')[0])
    f.close()
    return words


def gen_gaddag(filename):
    words = get_words(filename)
    initialState = Node()
    words = ['CARE', 'CARREL']
    i = 0
    for word in words:
        st = initialState
        n = len(word)
        # create path for anan-1...a1:
        for j in range(n - 1, 1, -1):
            st = st.add_arc(word[j])
        st = st.add_final_arc(word[1], word[0])

        # create path for an-1...a1^an
        st = initialState
        for j in range(n - 2, -1, -1):
            st = st.add_arc(word[j])

        st = st.add_final_arc('^', word[n - 1])

        for m in range(n - 3, -1, -1):
            forceSt = st
            st = initialState
            for j in range(m, -1, -1):
                st = st.add_arc(word[j])
            st = st.add_arc('^')
            st.force_arc(word[m + 1], forceSt)

        i += 1
        if (i % 5000 == 0):
            print i

    logger.info('States: %d, Arcs: %d' % (allocStates, allocArcs))
    save_gaddag('%s.gaddag' % filename)
    #newState = load_gaddag('%s.gaddag' % filename)
    #test_gaddag(words, newState)
    test_gaddag(words, initialState)


def generate_reprs_for_word(word):
    """ generate every rev(x)^y where xy is a word and x is not empty
        if y is empty, omit the ^"""
    length = len(word)
    retSet = set()

    for n in range(1, length + 1):
        s = ""
        s += word[0:n][::-1]    # [::-1] reverses a string
        if n == length:
            pass
        else:
            s += '^' + word[n:]
        retSet.add(s)
    return retSet


def get_all_paths(state, set, letters):
    lettersThisPath = copy.deepcopy(letters)
    for arc in state.arcs:
        arcLetter = state.arcs[arc].letter
        joined = ''.join(lettersThisPath + [arcLetter])
        for letter in state.arcs[arc].destNode.letterSet:
            set.add(joined + letter)
        get_all_paths(state.arcs[arc].destNode, set,
                      lettersThisPath + [state.arcs[arc].letter])


def test_gaddag(words, initialState):
    """ generates a set of all possible paths from "words", then generates
    all possible paths from initialState, and compares that the sets match"""

    actualSet = set()
    for word in words:
        actualSet.update(generate_reprs_for_word(word))

    print 'Actual set length:', len(actualSet)
    expectedSet = set()
    get_all_paths(initialState, expectedSet, [])
    print expectedSet
    if expectedSet != actualSet:
        print 'Did not pass'
    else:
        print 'Passed!'


def isUsed(arc):
    for node in nodes:
        if arc in node.arcs.values():
            return True

    return False


def save_gaddag(filename):
    serialized = []
    idx = 0

    print 'Nodes:'
    for node in nodes:
        print node

    for arc in arcs:
        if not isUsed(arc):
            print 'unused arc', arc

    for node in nodes:
        # represent each node as a bitvector of arcs and a bitvector for
        # letterset
        keys = node.arcs.keys()
        keys.sort()
        arcsBV = 0
        letterset = 0
        for key in keys:
            if key != '^':
                val = ord(key) - ord('A')
            else:
                val = 26
            arcsBV += (1 << val)
        serialized.append(arcsBV)
        node.idx = idx
        idx += 1
        for letter in node.letterSet:
            letterset += (1 << (ord(letter) - ord('A')))

        serialized.append(letterset)
        idx += 1

        for key in keys:
            assert(node.arcs[key].letter == key)
            serialized.append(node.arcs[key].destNode)
            idx += 1

    f = open(filename, 'wb')

    for val in serialized:
        if type(val) == int:
            towrite = val
        else:  # this is a node. write its index
            towrite = val.idx

        # write integer as little-endian, 4 bytes unsigned
        f.write(struct.pack('<I', towrite))

    f.close()

# def load_gaddag(filename):
#     """ loads a gaddag from file and returns the first node """
#     global allocArcs, allocStates, nodes, arcs
#     allocArcs = 0
#     allocStates = 0
#     nodes = []
#     arcs = []
#     f = open(filename, 'rb')
#     buf = f.read()
#     f.close()
#     # each node is a bitvector of arcs and a bitvector for letterset
#     # then each arc is just the index of the destination node
#     serialized = []
#     for idx in range(0, len(buf), 4):
#         serialized.append(struct.unpack_from('<I', buf[idx:idx + 4])[0])


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'usage: ./makegaddag.py wordlist'
    gen_gaddag(sys.argv[1])

