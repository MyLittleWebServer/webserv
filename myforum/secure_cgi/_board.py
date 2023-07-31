import pandas as pd
import multiprocessing
from secure_cgi.cgi_printer import CgiPrinter
from secure_cgi.jwt_cookie import JwtCookie
import uuid
import _datetime

file_lock = multiprocessing.Lock()

class Board:
	def __init__(self, id, jwt_cookie: JwtCookie, printer: CgiPrinter):
		self._id = id
		self._jwt_cookie : JwtCookie = jwt_cookie
		self._printer : CgiPrinter = printer

	def getData(self):
		df = self._read_csv_file("myforum/db/board.csv")
		id_condition = df["id"] == self._id
		result_df = df[id_condition]
		if (result_df.empty):
			self._fail()
		else:
			df.loc[id_condition, "view"] += 1
			result_df = df[id_condition]
			df.to_csv("myforum/db/board.csv", index=False)
			self._success(result_df)

	def _read_csv_file(self, file_path):
		with file_lock:
			df = pd.read_csv(file_path)
			return df

	def _save_csv_file(self, df):
		with file_lock:
			df.to_csv("myforum/db/board.csv", index=False)

	def _success(self, result_df):
		print("\r\n", end="")
		self.head()
		print(f"""<body>
			<div class="container">
				<h2>{result_df["title"].values[0]}</h2>
				<div>
					<p>작성자: {result_df["author"].values[0]}</p>
					<p>작성일: {result_df["date"].values[0]}</p>
					<p>조회수: {result_df["view"].values[0]}</p>
				</div>
				<hr/>""")
		if (result_df["picture"].values[0] != "" and result_df["picture"].values[0] != None and str(result_df["picture"].values[0]) != "nan"):
			print(f"""<div class="container">
				<img src="{result_df["picture"].values[0]}" class="mx-auto d-block" style="width:50%">
				</div>""")
		print(f"""<p>{result_df["content"].values[0]}</p>""")
		if (self._jwt_cookie._is_login() and self._jwt_cookie._get_username() == result_df["author"].values[0]):
			print(f"""<button type="button" class="btn btn-secondary" onClick="location.href='./deleteBoardService.py?id={self._id}'">삭제</button>""")
		self._bottom()

	def _bottom(self):
		print("""</body>
		<div class="footer-basic fixed-bottom">
			<div data-include-path="foot.html"></div>
		</div>""")
		print("""<style>
			body {
				min_height: 100%;
				margin: 0;
             	padding-bottom: 200px;
			}</style>""")
		print("</html>")
		print("""<script>
		window.addEventListener('load', function () {
			var allElements = document.getElementsByTagName('*');
			Array
				.prototype
				.forEach
				.call(allElements, function (el) {
					var includePath = el.dataset.includePath;
					if (includePath) {
						var xhttp = new XMLHttpRequest();
						xhttp.onreadystatechange = function () {
							if (this.readyState == 4 && this.status == 200) {
								el.outerHTML = this.responseText;
							}
						};
						xhttp.open('GET', includePath, true);
						xhttp.send();
					}
				});
		});
	</script>
	""")

	def head(self):
		print("""<!DOCTYPE html>
		<html>
		<head>
			<meta charset="UTF-8">
			<link rel="icon" href="/asset/index.ico/favicon.ico">
			<link
				href='http://www.openhiun.com/hangul/nanumbarungothic.css'
				rel='stylesheet'
				type='text/css'>
			<link rel="stylesheet" type="text/css" href="/asset/css/login.css">
			<link rel="stylesheet" type="text/css" href="/asset/css/index.css">
			<link
				rel="stylesheet"
				href="https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.1.3/css/bootstrap.min.css">
			<link
				rel="stylesheet"
				href="https://cdnjs.cloudflare.com/ajax/libs/ionicons/2.0.1/css/ionicons.min.css">
		</head>
		<header>
			<div data-include-path="nav.py"></div>
		</header>""")

	def _fail(self):
		print("\r\n", end="")
		self._cgi_printer._alertAndRedirect("./login.py", "Not Exist")

	def getBoards(self):
		df = self._read_csv_file("myforum/db/board.csv")
		self.head()
		print(f"""<body>
			<div class="container">
				<h1>게시판</h1>
				<table class="table">
					<thead class="thead-dark">
						<tr>
							<th>제목</th>
							<th>작성자</th>
							<th>작성일</th>
							<th>조회수</th>
						</tr>
					</thead>
					<tbody>""")
		for index, row in df.iterrows():
			id = row["id"]
			title = row["title"]
			author = row["author"]
			date = row["date"]
			view = row["view"]
			print(f"""<tr onclick="location.href='./board.py?id={id}'" style="cursor:pointer;">
								<td>{title}</td>
								<td>{author}</td>
								<td>{date}</td>
								<td>{view}</td>
							</tr>""")
		print("""</tbody>
					</table>""")
		if (self._jwt_cookie._is_login()):
			print("""<button type="button" class="btn btn-secondary" onClick="location.href='./postboard.html'">글쓰기</button>""")
		print("</div>")
		self._bottom()

	def writeBoard(self):
		self.head()
		print("""<body>
        <form class="container" action="./sign_upService.py" method="post">
            <div class="form-group">
                <label for="title">Title:</label>
                <input type="text" class="form-control" id="title">
            </div>
			<div class="custom-file">
				<input type="file" class="custom-file-input" id="customFile">
				<label class="custom-file-label" for="customFile">파일선택</label>
			</div>
            <div class="form-group">
                <label for="content">Content:</label>
                <textarea class="form-control" rows="5" id="content"></textarea>
            </div>
			<button type="submit" class="btn btn-primary">Sign-Up</button>
        </form>
    </body>""")
		self._bottom()

	def deleteBoard(self, username):
		df = self._read_csv_file("myforum/db/board.csv")
		username_condition = df["author"] == username
		id_condition = df["id"] == self._id
		result_df = df[username_condition & id_condition]
		df = df[~(username_condition & id_condition)]
		self._save_csv_file(df)
