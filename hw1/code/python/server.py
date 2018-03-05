from socket import *
import sys
import os
from threading import Thread
from threading import Lock
from concurrent_dict import conc_dict
from queue import Queue
from user import User
from select import select

from job import Job

byelock = Lock()
users = conc_dict()
shutdown = False
work_queue = Queue()
rsetsock = []
port_num = 0
num_workers = 0
motd = ""
server_commands = ["/users", "/help", "/shutdown"]
usage = ("./server [-hv] PORT_NUMBER NUM_WORKERS MOTD\n"
         "-h            Displays help menu & returns EXIT_SUCCESS.\n"
         "-v            Verbose print all incoming and outgoing protocol verbs & content.\n"
         "PORT_NUMBER   Port number to listen on.\n"
         "NUM_WORKERS   Number of workers to spawn.\n"
         "MOTD          Message to display to the client when they connect.")

arg_errormsg = "\n\nImproper arg format... Terminating Program."

# when server receives a /users command, dump contents of users to stdout
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

'''
def parse_command(input_src):
    command = (input_src.readline()).rstrip('\r\n ')
    if command == '/users':
        list_users()
    if command == '/help':
        help()
    if command == '/shutdown':
        server_shutdown(socket)
'''


def builtin_exec(command, sock):
    if command == '/users':
        list_users()
    elif command == '/help':
        help()
    elif command == '/shutdown':
        server_shutdown(sock)
    else:
        print(command, "is not a command")


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
    # disconnect all users, close open sockets
    # disconnect all users
    for key in users.list():
        tup = users.get(key)
        usock = tup[1]
        close_client(usock)
        # print(usock)
        # usock.shutdown(SHUT_RDWR)

    # shutdown main socket
    # sock.shutdown(SHUT_RDWR)
    sock.close()
    os._exit(0)


def parse_args(argc, argv):
    flags = 0
    if argc < 2:
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

    port_num = int(argv[1 + flags])
    num_workers = int(argv[2 + flags])
    motd = (argv[3 + flags])
    return (port_num, num_workers, motd)


def get_name_by_sock(sock):
    for name in users.list():
        if users.get(name)[1] == sock:
            return name

    else:
        return ""


def handle_bye(sock):
    # get the name of the person logging off and delete the socket
    # global byelock
    # byelock.acquire()
    logoff = get_name_by_sock(sock)

    # send EYB and close socket, delete from user dict
    sock.send("EYB\r\n\r\n".encode())
    users.delete(logoff)
    close_client(sock)

    # send UOFF messages to every other client
    for name in users.list():
        socksnd = users.get(name)[1]
        sndmsg = 'UOFF ' + logoff + "\r\n\r\n"
        socksnd.send(sndmsg.encode())
    # byelock.release()


def handle_listu(sock):
    message = "UTSIL " + " ".join(users.list()) + "\r\n\r\n"
    sock.send(message.encode())


def handle_to(sock: socket, message: str):
    # get the name of the client who this socket belongs to
    fromname = get_name_by_sock(sock)
    print("From:", fromname)

    # Decode the message from <dest> <mesg> and send it to the dest client
    splitmessage = message.split(" ", maxsplit=1)
    print("Splitmessage:", splitmessage)
    toname = splitmessage[0]
    if(users.get(toname) != None):
        destsock = users.get(toname)[1]
    else:
        edne = "EDNE " + toname + "\r\n\r\n"
        sock.send(edne.encode())
        return 0
    
    # read the whole mesage and write to the dest
    frommsg = "FROM " + fromname + " " + splitmessage[1]
    while "\r\n\r\n" not in frommsg:
        frommsg = frommsg + (sock.recv(32)).decode("utf-8")
    destsock.send(frommsg.encode())
    return 0


def close_client(sock):
    global rsetsock
    if sock in rsetsock:
        rsetsock.remove(sock)
    sock.close()


def login(addr, sock):
    init = sock.recv(32).decode('utf-8')
    if init == 'ME2U\r\n\r\n':
        sock.send('U2EM\r\n\r\n'.encode())
        init2 = sock.recv(32).decode('utf-8')
        i2l = init2.split(" ")
        if i2l[0] == 'IAM':
            if i2l[1].endswith("\r\n\r\n"):
                name = i2l[1].rstrip("\r\n")
                if len(name) <= 10:
                    if users.get(name) is None:
                        users.put(name, (addr, sock))
                        sock.send("MAI\r\n\r\n".encode())
                        motdstring = "MOTD " + motd + "\r\n\r\n"
                        sock.send(motdstring.encode())
                        global rsetsock
                        rsetsock.append(sock)
                        return 0
                    else:
                        sock.send("ETAKEN\r\n\r\n".encode())
    sock.close()
    return 0


def handle_morf(sock, message: str):
    fromname = get_name_by_sock(sock)
    toname = message.rstrip("\r\n")
    destsock = users.get(toname)[1]
    ot = "OT " + fromname + "\r\n\r\n"
    destsock.send(ot.encode())
    return 0


def client_read(info, connection):
    # Read from client socket
    # info = client address
    # connection = client socket
    # users are keyed by connection. Verify that destination of message is a user, else EDNE
    if get_name_by_sock(connection) is None:
        return None
    try:
        message = (connection.recv(32, MSG_DONTWAIT)).decode()
    except BlockingIOError:
        return 0
    except OSError:
        return None
    header = message.split(sep=" ", maxsplit=1)
    if header[0] == "LISTU\r\n\r\n":
        #send UTSIL + list of users + \r\n\r\n to client
        handle_listu(connection)

    elif header[0] == "TO":
        # apply messaging protocol.
        # identify receiver in users dictionary.
        # send FROM <sender> <message>\r\n\r\n if recipient exists and message is not garbage
        print("TO")
        handle_to(connection, header[1])

    elif header[0] == "BYE\r\n\r\n":
        # ack with EYB\r\n\r\n and send UOFF <username>\r\n\r\n to rest of the users online
        handle_bye(connection)
        return None
    elif header[0] == "MORF":

        handle_morf(connection, header[1])

    elif header[0] != "":
        # message does not have a valid header and is garbage
        print("Protocol error:", header[0], "is not a recognized client command")
        name = get_name_by_sock(connection)
        if name is not None:
            users.delete(name)
            connection.close()
        return None


    return 0


def worker_exec(i, socket):
    # worker threads will execute this when started
    print("Spawned worker thread #", i)
    global byelock
    while True:
        job = work_queue.get()

        if job.type == "BUILT-IN":
            builtin_exec(str(job.info).strip('\n'), socket)

        elif job.type == "LOGIN":
            login(job.info, job.connection)

        elif job.type == 'CLIENT':
            global rsetsock
            if job.connection in rsetsock:
                byelock.acquire()
                rsetsock.remove(job.connection)
                if client_read(job.info, job.connection) is not None:
                    rsetsock.append(job.connection)
                byelock.release()

        else:
            print("error")


if __name__ == '__main__':
    # store logged in users as a dictionary to allow for iterative dumps and fast insertion/collision checking

    argc = len(sys.argv)

    port_num, num_workers, motd = parse_args(argc, sys.argv)

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
        worker = Thread(target=worker_exec, args=(x, sock,))
        threads.append(worker)
        worker.start()

    # rsockset = []
    while not shutdown:  # change this later
        try:
            rset = [sock, sys.stdin] + rsetsock# select listening socket or stdin to read
            #wset = [sock]  # will be filled later
            eset = [sock]  # not defined yet
            # for name in users.list():
               # print(name)
                # rset.append(users.get(name)[1])
            tout = 1
            r_ready,w_ready, e_ready = select(rset, [],eset, 0)
            # print("Ready for I/O")
            for r in r_ready:
                if r == sock:
                    connection = sock.accept()
                    new_job = Job("LOGIN", connection[1], connection[0])
                    work_queue.put(new_job)
                    # rsetsock.append(connection[0])  # we will be able to read from the connection

                    # WE dont need to multiplex over writing, just write to the socket
                    # wset.append(connection[0]) #we will also write to the connection

                    # recv_handler(connection) not implemented

                if r == sys.stdin:
                    # parse_command(sys.stdin)
                    input = (sys.stdin.readline()).rstrip('\r\n')
                    print("Input:", input)
                    work_queue.put(Job("BUILT-IN", input, None))
                if isinstance(r, socket) and r != sock:
                    # recv_handler(r)
                    if r.fileno() > 0:
                        work_queue.put(Job("CLIENT", "", r))

        except timeout:
            sock.close()
            for t in threads:
                t.join()
            print("Timeout reached. Terminating program.")
            exit(1)
        except OSError as e:
            print("Error ", e.errno, e.strerror)
            exit(1)

    print("Server shutting down")
    for t in threads:
        t.join()
    exit(0)
