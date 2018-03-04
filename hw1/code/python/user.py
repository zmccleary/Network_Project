class User:
    def __init__(self, name):
        self.name = name
        self.messages = []

    def get_messages(self):
        return self.messages
