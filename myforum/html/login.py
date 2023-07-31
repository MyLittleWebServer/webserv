#!/Users/chanheki/.brew/bin/python3.11

import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from secure_cgi.cgi_printer import CgiPrinter
from secure_cgi.jwt_cookie import JwtCookie

if __name__ == "__main__":
	cgi_printer: CgiPrinter = CgiPrinter()
	jwt_cookie: JwtCookie = JwtCookie()
	cgi_printer._print_status_ok_header()
	cgi_printer._print_crlf()
	if jwt_cookie._is_login():
		cgi_printer._alertAndRedirect("./index.html", "Already Login")
	print("""<!DOCTYPE html>
    <html>
        <head>
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

        <body>
            <div data-include-path="nav.py"></div>
            <form class="container" action="./loginService.py" method="post">
                <div class="button-wrapper">
                    <h2>Login</h2>
                </div>
                <label for="username">
                    <b>Username</b>
                </label>
                <input
                    type="text"
                    placeholder="Enter Username"
                    name="username"
                    required="required">

                <label for="password">
                    <b>Password</b>
                </label>
                <input
                    type="password"
                    placeholder="Enter Password"
                    name="password"
                    required="required">
                <button type="submit" class="btn">Login</button>
                <button
                    type="button"
                    class="btn btn-primary"
                    onclick="window.location.href='./sign_up.html';">Sign-Up</button>
            </form>
        </body>

        <div class="footer-basic fixed-bottom">
            <div data-include-path="foot.html"></div>
        </div>

        <script>
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
    </html>""")
