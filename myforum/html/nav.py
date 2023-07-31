#!/opt/homebrew/bin/python3

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from secure_cgi.cgi_printer import CgiPrinter
from secure_cgi.jwt_cookie import JwtCookie

if __name__ == "__main__":
	cgi_printer: CgiPrinter = CgiPrinter()
	jwt_cookie: JwtCookie = JwtCookie()
	cgi_printer._print_status_ok_header()
	is_login : bool = jwt_cookie._is_login()
	cgi_printer._print_crlf()

	print("""<head>
			<title>Tokatoka</title>
		</head>
		<div class="navbar">
		<a href="./index.html">Home</a>
		<a href="./boards.py">Board</a>
		<a href="./gaepo.html">About</a>""")
	if (is_login):
		print(f"""
		<a href="./logoutService.py">Logout</a>
	</div>""")
	else:
		print("""
			<a href="./login.py">Login</a>
		</div>""")
