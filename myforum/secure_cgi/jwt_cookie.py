import jwt
import datetime
import os


class JwtCookie:
	def __init__(self):
		self._secret_key = "my_forum_secret_key"
		self._time = 30

	def _make_cookie(self, username):
		token = self._makeJwt(username)
		# expires_time = datetime.datetime.utcnow() + datetime.timedelta(minutes=(self._time * 10))
		# expires_formatted = expires_time.strftime('%a, %d %b %Y %H:%M:%S GMT')
		# print(f"Set-Cookie: auth={token}; path=/; httponly;  expires={expires_formatted}", end="\r\n")
		print(f"Set-Cookie: auth={token}; path=/; httponly;", end="\r\n")

	def _delete_cookie(self):
		print("Set-Cookie: auth=; path=/; httponly;  expires=Thu, 01 Jan 1970 00:00:00 GMT", end="\r\n")

	def _makeJwt(self, username):
		payload = {
			'username': username,
			'exp': datetime.datetime.utcnow() + datetime.timedelta(minutes=self._time)
		}
		token = jwt.encode(payload, self._secret_key, algorithm='HS256')
		return token

	def _get_username(self):
		auth = list(os.environ.get('HTTP_COOKIE').split(';'))
		if auth[0] == "":
			return ""
		cookie_dict = {key.strip(): value.strip() for key, value in (item.split('=') for item in auth)}
		auth = cookie_dict.get("auth", "")
		if auth	== "":
			return ""
		try:
			return jwt.decode(auth, self._secret_key, algorithms=['HS256'])["username"]
		except:
			self._delete_cookie()
			return ""

	def _is_login(self):
		auth = list(os.environ.get('HTTP_COOKIE').split(';'))
		if auth[0] == "":
			return False
		cookie_dict = {key.strip(): value.strip() for key, value in (item.split('=') for item in auth)}
		auth = cookie_dict.get("auth", "")
		if auth	== "":
			return False
		try:
			jwt.decode(auth, self._secret_key, algorithms=['HS256'])
			return True
		except:
			self._delete_cookie()
			return False
