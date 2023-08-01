#!/Users/chanheki/.brew/bin/python3.11

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import pandas as pd
import multiprocessing
from secure_cgi.jwt_cookie import JwtCookie
from secure_cgi.cgi_printer import CgiPrinter

file_lock = multiprocessing.Lock()


class SignUp:
	def __init__(self, username, password, jwt_cookie: JwtCookie, printer: CgiPrinter):
		self._username = username
		self._password = password
		self._jwt_cookie : JwtCookie = jwt_cookie
		self._printer : CgiPrinter = printer

	def sign_up(self):
		df = self._read_csv_file("myforum/db/user.csv")
		username_condition = df["username"] == self._username
		result_df = df[username_condition]
		if (len(self._username) < 5):
			self._fail("Username must be at least 5 characters")
		if (len(self._password) < 5):
			self._fail("Password must be at least 5 characters")
		if (len(self._username) > 15):
			self._fail("Username must be at max 15 characters")
		if (len(self._password) > 15):
			self._fail("Password must be at max 15 characters")
		if (not result_df.empty):
			self._fail("Username already exists")
		else:
			new_data = {'username': self._username, 'password': self._password}
			df = pd.concat([df, pd.DataFrame([new_data])], ignore_index=True)
			df.to_csv("myforum/db/user.csv", index=False)
			self._success()

	def _read_csv_file(self, file_path):
		with file_lock:
			df = pd.read_csv(file_path)
			return df

	def _success(self):
		self._jwt_cookie._make_cookie(self._username)
		print("\r\n", end="")
		cgi_printer._alertAndRedirect("./index.html", "Sign-up Success")
		exit()

	def _fail(self, msg):
		print("\r\n", end="")
		cgi_printer._alertAndRedirect("./sign_up.html", msg)
		exit()


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
	sign_up : SignUp = SignUp(username, password, jwt_cookie, cgi_printer)
	cgi_printer._print_status_ok_header()
	sign_up.sign_up()
