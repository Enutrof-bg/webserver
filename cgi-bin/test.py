#!/usr/bin/env python3
import os
import sys
import time
method = os.environ.get('REQUEST_METHOD', '')
query_string = os.environ.get('QUERY_STRING', '')
path_info = os.environ.get('PATH_INFO', '')
content_length = int(os.environ.get('CONTENT_LENGTH', 0))

# time.sleep(2)
# print("Content-Type: text/html\r\n\r\n", end='')
print("<html><body>")
print(f"<h1>Method: {method}</h1>")
print(f"<p>Query String: {query_string}</p>")
print(f"<p>Path Info: {path_info}</p>")
print(f"<p>Content Length: {content_length}</p>")
# time.sleep(3)
if method == 'POST' and content_length > 0:
    # Lire le body depuis STDIN
    post_data = sys.stdin.read(content_length)
    print(f"<p>POST Data: {post_data}</p>")
print("<a title=\"test\" href=\"/\">go back</a></p>")
print("</body></html>")