import sys
import zmq

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage: python comm.py {build|anagram} tiles"
        sys.exit()
    context = zmq.Context()
    socket = context.socket(zmq.REQ)
    socket.connect('tcp://localhost:4000')
    socket.send('%s %s' % (sys.argv[1], sys.argv[2]))
    message = socket.recv()
    print message
