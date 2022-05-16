#!/usr/local/bin/python3

import time

time.sleep(5)

print("HTTP/1.1 200 OK")
print("Content-type:text/html\r\n")
print("<h1>Feeling sleepy...</h1>")
