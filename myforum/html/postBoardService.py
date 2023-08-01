#!/Users/chanheki/.brew/bin/python3.11

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from secure_cgi.cgi_printer import CgiPrinter
from secure_cgi.jwt_cookie import JwtCookie
from secure_cgi._board import Board
import json

import multiprocessing
import uuid
import _datetime
import pandas as pd


file_lock = multiprocessing.Lock()

def read_csv_file(self, file_path):
	with file_lock:
		df = pd.read_csv(file_path)
		return df

if __name__ == "__main__":
	cgi_printer: CgiPrinter = CgiPrinter()
	jwt_cookie: JwtCookie = JwtCookie()
	board = Board(0, jwt_cookie, cgi_printer)

	is_login : bool = jwt_cookie._is_login()
	title = ""
	location = ""
	content = ""
	author = ""

	# if (is_login == False):
	# 	cgi_printer._print_status_error_header()
	# 	cgi_printer._print_crlf()
	# 	sys.exit()

	try:
		data = json.loads(sys.stdin.read())
		title = data.get("title")
		location = data.get("location")
		content = data.get("content")
		author = jwt_cookie._get_username()
		df = pd.read_csv("myforum/db/board.csv")
		new_id = uuid.uuid4()
		new_record = {
            "id": new_id,
            "author": author,
            "title": title,
            "content": content,
            "view": 0,
            "date": _datetime.datetime.now(),
            "picture": location
        }
		df = pd.concat([df, pd.DataFrame([new_record])], ignore_index=True)
		df.to_csv("myforum/db/board.csv", index=False)

	except:
		cgi_printer._print_status_error_header()
		cgi_printer._print_crlf()
		sys.exit()


	cgi_printer._print_status_ok_header()
	cgi_printer._print_crlf()

