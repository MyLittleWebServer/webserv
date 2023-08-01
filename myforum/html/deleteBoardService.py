#!/Users/chanheki/.brew/bin/python3.11

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from secure_cgi.cgi_printer import CgiPrinter
from secure_cgi.jwt_cookie import JwtCookie
from secure_cgi._board import Board


if __name__ == "__main__":
	cgi_printer: CgiPrinter = CgiPrinter()
	jwt_cookie: JwtCookie = JwtCookie()
	cgi_printer._print_status_ok_header()
	cgi_printer._print_crlf()
	query = list(os.environ.get('QUERY_STRING').split('&'))
	id = query[0].split('=')[1]
	is_login : bool = jwt_cookie._is_login()
	username = jwt_cookie._get_username()
	if query[0] == "" or (not is_login):
		cgi_printer._alertAndRedirect("./index.html", "bad access!")
	else:
		board = Board(id, jwt_cookie, cgi_printer)
		board.deleteBoard(username)
		cgi_printer._alertAndRedirect("./boards.py", "delete success!")

