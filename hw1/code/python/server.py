from socket import *
import sys
from threading import Thread
from concurrent_dict import conc_dict
from queue import Queue
from user import User
from select import select

from job import Job


users = conc_dict()

work_queue = Queue()
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
'''def recv_handle(conn, bufsize, buf):
    bytes_read = None
    conn_sock = conn[0]
    conn_addr = conn[1]
    try:
        bytes_read =conn[0].recv(bufsize)
        string_buf = bytes_read.decode().split(sep ="/r/n") 
        index = 0
        formatter = []
        terminal = False
        #create a list of strings, where any instances of only single /r/n are concatenated with following string
        #This will enable us to check for garbage later.
        for s in string_buf:
            if s == "":
                terminal = True
            if terminal == True:
                formatter.append("/r/n/r/n")
                index += 1
            else:
                formatter[index] + s
        
        for s in formatter:
            if s.endswith('/r/n/r/n'):
                work_queue.put(s)
            elif formatter.index(s) == len(formatter) - 1:
                buf.append(s) #if improper string is the last element of the list, then we will store it and see if it gets completed later.
            else:
                queue.put("Delete " + users.get(conn_addr)) #else, we send a delete request to the work queue to delete the client connection (keyed by address).
        return None
'''
def builtin_exec(command, sock):
    if command == '/users':
        list_users()
    elif command == '/help':
        help()
    elif command == '/shutdown':
        shutdown(sock)
    else:
        print('Please enter a valid command.')

# NOTE: may not be thread-safe, implement locks
def list_users():
    dump_list = users.list()
    print(dump_list)
   # for name in dump_list:
    #    print(name[0], end = ", ")
    print("")

def help():
    for s in server_commands:
        print(s, end="; ")
    print("")

def server_shutdown(sock):
    #disconnect all users, close open sockets
    sock.shutdown(SHUT_RDWR)

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

def worker_exec(i, socket):
    #worker threads will execute this when started
    print("Spawned worker thread #",i)
    job = work_queue.get()

    if job.type == "BUILT-IN" : 
        builtin_exec(job.info, socket)

    elif job.type == "LOGIN" :
       # login()
    elif job == 'CLIENT':
       # client_read(job.info, job.connection)
    else:
        print("error")
if __name__ == '__main__':
     #store logged in users as a dictionary to allow for iterative dumps and fast insertion/collision checking

    argc = len(sys.argv)

    port_num, num_workers, motd = parse_args(argc,sys.argv)
   
    try:
        sock = socket()

    except OSError:
        print("Unable to create socket instance.")
        exit(1)
    try:
        sock.bind(('', port_num))
        sock.listen()
        sock.settimeout(10)
    except OSError:
        print("Unable to complete bind/listen.")
        exit(1)
    except herror as e:
        print("Hostname error", e.errno)
        exit(1)
    threads = []
    for x in range(num_workers):
        worker = Thread(target = worker_exec, args =(x,sock,))
        threads.append(worker)
        worker.start()
    
    while True: #change this later
        try:
            rset = [sock, sys.stdin] #select listening socket or stdin to read
            wset = [sock] #will be filled later
            eset = [sock] #not defined yet
            r_ready,w_ready, e_ready = select(rset,wset,eset, 30)
            print("Ready for I/O") 
            for r in r_ready:
                if r == sock:

                   connection = sock.accept()
                   '''work_queue.put(Job("LOGIN", connection[1],str(connection[0])))"'''
                   recv_handler(connection)
                if r == sys.stdin:
                    parse_command(sys.stdin)
                    '''work_queue.put(Job("BUILT-IN",readline(sys.stdin).rsplit('\r\n')))'''
                elif isinstance(r, tuple) and isinstance(r[0], socket.socket):
                    recv_handler(r)
                    '''work_queue.put(Job("CLIENT",r[1],r[0]))'''
            rset.append(connection[0]) #we will be able to read from the connection
            wset.append(connection[0]) #we will also write to the connection

                    connection = sock.accept()
                    recv_handler(connection)
                    rset.append(connection[0]) #we will be able to read from the connection
                    wset.append(connection[0]) #we will also write to the connection
            
                if r == sys.stdin:
                    parse_command(sys.stdin)
                elif isinstance(r, tuple) and isinstance(r[0], socket):
                    recv_handler(r)

            
            
       
        except timeout:
            sock.close()
            for t in threads:
                t.join()
            print("Timeout reached. Terminating program.")
            exit(1)
        except OSError as e:
            print("Error ", e.errno, e.strerror)
            exit(1)

            
