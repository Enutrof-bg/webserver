#!/usr/bin/env python3

import os
import sys

method = os.environ.get('REQUEST_METHOD', '')
content_length = int(os.environ.get('CONTENT_LENGTH', 0))


time.sleep(3)
print("Content-Type: text/html\r\n\r\n")
print("<html><body>")
print("<h1>Hello from CGI!</h1>")
if method == 'POST' and content_length > 0:
    # Lire le body depuis STDIN
    post_data = sys.stdin.read(content_length)
    print(f"<p>POST Data: {post_data}</p>")
print("<a title=\"test\" href=\"/\">go back</a></p>")
print("</body></html>")

for cle, valeur in os.environ.items():
    print(f"{cle} = {valeur}", file=sys.stderr)
