class CgiPrinter:
	def __init__(self):
		pass

	def _print_status_ok_header(self):
		print ("Status: 200 OK\r\n", end="")
		print ("Content-Type: text/html; charset=UTF-8\r\n", end="")

	def _print_status_error_header(self):
		print ("Status: 500 INTERNAL SERVER ERROR\r\n", end="")
		print ("Content-Type: text/html; charset=UTF-8\r\n", end="")

	def _print_crlf(self):
		print("\r\n", end="")

	def _bad_request(self):
		print ("Status: 400 Bad Request\r\n", end="")
		print ("Content-Type: text/plain; charset=UTF-8\r\n\r\n", end="")
		print ("Bad Request")

	def _alertAndRedirect(self, redirect_page: str , message: str):
		print(f"""<!DOCTYPE html>
	<html>
	<head>
	<title>Please Wait...</title>
	</head>
	<body>
		<h1>Now Loading...</h1>
	<script>
		window.location.href = "{redirect_page}";
		alert("{message}");
	</script>
	</body>
	</html>""")
