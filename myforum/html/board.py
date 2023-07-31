#!/opt/homebrew/bin/python3

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
	cookie_dict = {key.strip(): value.strip() for key, value in (item.split('=') for item in query)}
	id : str = cookie_dict.get("id", "")
	if query[0] == "":
		exit()
	board = Board(id, jwt_cookie, cgi_printer)
	board.getData()
