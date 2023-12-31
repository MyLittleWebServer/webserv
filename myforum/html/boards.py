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
	is_login : bool = jwt_cookie._is_login()
	cgi_printer._print_crlf()
	board = Board(0, jwt_cookie, cgi_printer)
	board.getBoards()

