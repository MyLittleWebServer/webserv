#!/opt/homebrew/bin/python3

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from secure_cgi.jwt_cookie import JwtCookie
from secure_cgi.cgi_printer import CgiPrinter

if __name__ == "__main__":
	print ("Status: 200 OK\r\n", end="")
	jwt_cookie : JwtCookie = JwtCookie()
	cgi_printer: CgiPrinter = CgiPrinter()
	jwt_cookie._delete_cookie()
	cgi_printer._print_crlf()
	cgi_printer._alertAndRedirect("./index.html", "Logout Success")
