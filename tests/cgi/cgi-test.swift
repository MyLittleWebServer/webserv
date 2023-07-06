#!/usr/bin/swift

import Foundation

print("Content-Type: text/html\n\n")
print("<html><body>")
print("<h1>CGI Test</h1>")
print("<p>Environment Variables:</p>")
print("<table>")
for (key, value) in ProcessInfo.processInfo.environment {
    print("<tr><td>\(key)</td><td>\(value)</td></tr>")
}
print("</table>")
print("<p>Arguments:</p>")
print("<table>")
for arg in CommandLine.arguments {
    print("<tr><td>\(arg)</td></tr>")
}
print("</table>")
print("</body></html>")

