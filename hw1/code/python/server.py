import socket
import sys
from multiprocessing import Process

flags = 0
port_num = 0
num_workers = 0
motd = ""


usage =("./server [-hv] PORT_NUMBER NUM_WORKERS MOTD\n"
"-h            Displays help menu & returns EXIT_SUCCESS.\n"
"-v            Verbose print all incoming and outgoing protocol verbs & content.\n"
"PORT_NUMBER   Port number to listen on.\n"
"NUM_WORKERS   Number of workers to spawn.\n"
"MOTD          Message to display to the client when they connect.")

arg_errormsg = "Improper arg format... Terminating Program."

users = {} #store logged in users as a dictionary to allow for iterative dumps and fast insertion/collision checking

argc = len(sys.argv)

if argc < 1:
    print(usage, arg_errormsg)
    exit(1)

if sys.argv[1] == "-h":
    print(usage)
    exit(0)

if sys.argv[1] == '-v':
    flags = 1

if argc - flags != 4:
    print(usage, arg_errormsg)
    exit(1)
port_num = int(sys.argv[1 + flags])
num_workers = int(sys.argv[2+flags])
motd = (sys.argv[3+flags])
try:
    sock = socket.socket()

except OSError:
    print("Unable to create socket instance.")
try:
    sock.bind(sock.getfqdn(), port_num)
    sock.listen()
except OSError:
    print("Unable to complete bind/listen.")
except socket.herror as e:
    print("Hostname error", e.errno) 

while True: #change this later
    try:
        connection = sock.accept()
        p = Process(target=serverExec)
        p.start("fork")
        p.join()
    except OSError as e:
        print("Error ", e.errno)
    finally:
        sock.close()
