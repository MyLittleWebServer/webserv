#!/usr/bin/python3

import os

print ("Content-Type: text/html\r")
print ("\r")
print ("<!DOCTYPE html>")
print ("<html>")
print ("<head>")
print ("<title>decoded text</title>")
print ("</head>")
print ("<body><h1>")
print ("caesar(plain_text)")
print ("<table>")
for key, val in os.environ.items():
    print("<tr><td>", key, "</td><td>", val, "</td></tr>")
print ("</table>")
print ("</h1></body>")
print ("</html>")