import socket
import sys
from threading import Thread
from concurrent_dict import conc_dict


port_num = 0
num_workers = 0
motd = ""
server_commands = ["/users","/help", "/shutdown"]
usage =("./server [-hv] PORT_NUMBER NUM_WORKERS MOTD\n"
    "-h            Displays help menu & returns EXIT_SUCCESS.\n"
    "-v            Verbose print all incoming and outgoing protocol verbs & content.\n"
    "PORT_NUMBER   Port number to listen on.\n"
    "NUM_WORKERS   Number of workers to spawn.\n"
    "MOTD          Message to display to the client when they connect.")

arg_errormsg = "\n\nImproper arg format... Terminating Program."

#when server receives a /users command, dump contents of users to stdout

# NOTE: may not be thread-safe, implement locks
def users():
    print(list(users.keys))

def help():
    for s in server_commands:
        print(s, end="; ")
    print("")

def server_shutdown():
    #disconnect all users, close open sockets
    return None

def parse_args(argc, argv):
    flags = 0
    if argc < 2:
        print(usage,arg_errormsg)
        exit(1)

    if sys.argv[1] == "-h":
        print(usage)
        exit(0)

    if sys.argv[1] == '-v':
        flags = 1

    if argc - flags != 4:
        print(usage,arg_errormsg)
        exit(1)
    
    port_num = int(argv[1 + flags])
    num_workers = int(argv[2+flags])
    motd = (argv[3+flags])
    return (port_num, num_workers,motd)
def worker_exec(i):
    #worker threads will execute this when started
    print("Spawned worker thread #%d",i)
if __name__ == '__main__':
    users = conc_dict() #store logged in users as a dictionary to allow for iterative dumps and fast insertion/collision checking

    argc = len(sys.argv)

    port_num, num_workers, motd = parse_args(argc,sys.argv)
   
    try:
        sock = socket.socket()

    except OSError:
        print("Unable to create socket instance.")
    try:
        sock.bind(('', port_num))
        sock.listen()
        sock.settimeout(30)
    except OSError:
        print("Unable to complete bind/listen.")
    except socket.herror as e:
        print("Hostname error", e.errno)
    threads = []
    for x in range(num_workers):
        worker = Thread(target = worker_exec, args =(x,))
        threads.append(worker)
        worker.start()
    
    while True: #change this later
        try:
            connection = sock.accept()
       
        except socket.timeout:
            sock.close()
            for t in threads:
                t.join()
            print("Timeout reached. Terminating program.")
            exit(1)
        except OSError as e:
            print("Error ", e.errno)

            
