The `gaddag` module creates a [GADDAG](http://en.wikipedia.org/wiki/GADDAG) and saves it to disk.

The save structure looks like a series of 32-bit integers in the following format:

Word 0: The number of total elements (nodes * 2 + arcs)
Word 1: The number of total nodes.
Then, for every node:
- 32-bit Arc bit vector; the bit value is 1 for each arc coming out of a node (each arc has a letter associated with it). Lowest bit is A, highest is the ^ separator.
- 32-bit letter set bit vector; if a node has a letter set this is a terminal node for each of those letters.
- A series of 32-bit numbers, one for each arc in the node, alphabetically:
  - Each 32-bit number is the index of the node this arc points to. Note that when counting indexes, skip arcs, we're just counting nodes.