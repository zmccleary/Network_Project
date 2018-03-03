from threading import Semaphore
#implement readers-writers pattern for modifying/accessing a Python dictionary

'''Readers-Writers Problem:
    A data structure has two types of functions: 
        1) Readers - Functions that look at the structure and return a value without modifying the contents
        2) Writers - Functions that modify the contents of the underlying structure
        
    This library will implement functions that prioritize writes over reads. That means we have the following conditions to account for:
        *A Read will only execute as long as there is no Write scheduled. 
        *We will keep a count of the # of Write functions blocking using a counting semaphore. 
            -[# writers]++ whenever a thread calls a write function.
            -[# writers]-- whenever critical code of write function finishes for a thread.
            -While [# writers] > 0, function will hold the lock_sem. Only when there are no more Writers waiting, will
            the lock_sem be released.
        *Read functions must wait to acquire lock_sem. Reads may be starved if writes continue to come in.'''

class conc_dict:
    #this class defines a dictionary that is thread-safe
    
    ''' self.lock        ------ Semaphore init to 1. All functions in class will need to acquire() in order to access critical code. 
        self.num_readers ------ Counting Semaphore. Read functions will call release() to increment counter
        self.dict        ------ Dictionary object that will be used to store data'''
    
    def __init__(self):
        self.lock = Semaphore()
        self.num_readers = Semaphore(0)
        self.read_sem = Semaphore()
        data = {}

    def put(self, key, value):
        self.lock.acquire()
        self.data[key] = value
        self.lock.release()

    def get(self, key):
        self.read_sem.acquire() #prevent other readers from locking when updating num_readers
        self.num_readers.release() #increment num_readers
        
        if self.num_readers == 1: #if first reader then grab the dictionary lock
            self.lock.acquire()
        self.read_sem.release()  #Reader info has been successfully updated and other read functions can unblock     
        
        if key in self.dict:
            returnValue = self.data[key]
        else:
            returnValue = None
        
        self.num_readers.acquire() #Decrement num_readers after reading from dict
        
        if self.num_readers == 0: #if last reader, then release the resource lock for other functions
            self.lock.release()
 
        return returnValue



    def delete(self, key):
        self.lock.acquire()
        del self.data[key]
        self.lock.release()

    def contains(self, key):
        if get(key) == True:
            return True
        else:
            return False
