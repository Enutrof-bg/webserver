#!/usr/bin/env python3

import os
import sys
print("Content-Type: text/html\r\n\r\n")
print("<html><body>")
print("<h1>Hello from CGI!</h1>")
print("<a title=\"test\" href=\"/\">go back</a></p>")
print("</body></html>")

for cle, valeur in os.environ.items():
    print(f"{cle} = {valeur}", file=sys.stderr)
