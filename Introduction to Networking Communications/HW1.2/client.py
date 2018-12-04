import socket


def Main():
    # local host IP '127.0.0.1'
    host = '127.0.0.1'

    # Define the port on which you want to connect
    port = 9251

    s = socket.socket()

    # connect to server on local computer
    s.connect((host, port))

    while True:

        message = input("What's the message you want to send to the server? ")
        # message sent to server
        s.send(message.encode('ascii'))

        # messaga received from server
        data = s.recv(1024)

        # print the received message
        print('Received from the server :', str(data.decode('ascii')))

        # ask the client whether he wants to continue
        ans = input('\nDo you want to continue(y/n) :')
        if ans == 'y':
            continue
        else:
            break
    # close the connection
    s.close()


if __name__ == '__main__':
    Main()
