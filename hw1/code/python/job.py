import socket
class Job:
    #Create class of Job objects to be Queue'd by main server process and deciphered by worker threads
    
    def __init__(self, job_type, info, connection: socket):
        self.type = job_type
        self.info = str(info)
        self.connection = connection

