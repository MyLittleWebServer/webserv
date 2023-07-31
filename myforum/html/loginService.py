#!/opt/homebrew/bin/python3

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import pandas as pd
import multiprocessing
from secure_cgi.jwt_cookie import JwtCookie
from secure_cgi.cgi_printer import CgiPrinter


file_lock = multiprocessing.Lock()


class Login:
	def __init__(self, username, password, jwt_cookie: JwtCookie, printer: CgiPrinter):
		self._username = username
		self._password = password
		self._jwt_cookie : JwtCookie = jwt_cookie
		self._printer : CgiPrinter = printer

	def login(self):
		df = self._read_csv_file("myforum/db/user.csv")
		username_condition = df["username"] == self._username
		password_condition = df["password"] == self._password
		result_df = df[username_condition & password_condition]
		if (result_df.empty):
			self._fail()
		else:
			self._success()

	def _read_csv_file(self, file_path):
		with file_lock:
			df = pd.read_csv(file_path)
			return df

	def _success(self):
		self._jwt_cookie._make_cookie(self._username)
		print("\r\n", end="")
		cgi_printer._alertAndRedirect("./index.html", "Login Success")

	def _fail(self):
		print("\r\n", end="")
		cgi_printer._alertAndRedirect("./login.py", "Login Failed")


if __name__ == "__main__":
	data = list(str(sys.stdin.read()).split('&'))
	cgi_printer: CgiPrinter = CgiPrinter()
	if (len(data) != 2):
		cgi_printer._bad_request()
		exit()

	username = data[0].split('=')[1]
	password = data[1].split('=')[1]

	if (data[0].split('=')[0] != "username" or data[1].split('=')[0] != "password"):
		cgi_printer._bad_request()
		exit()

	jwt_cookie : JwtCookie = JwtCookie()
	login : Login = Login(username, password, jwt_cookie, cgi_printer)
	cgi_printer._print_status_ok_header()
	login.login()
