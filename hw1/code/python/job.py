class Job:
    #Create class of Job objects to be Queue'd by main server process and deciphered by worker threads
    
    def __init__(self, job_type, info, connection=None):
        self.type = job_type
        self.info = info
        self.connection = connection

