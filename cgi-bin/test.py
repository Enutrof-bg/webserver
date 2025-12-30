#!/usr/bin/env python3
import os
import sys

script_name = os.environ.get('SCRIPT_NAME', '')
path_info = os.environ.get('PATH_INFO', '')
query_string = os.environ.get('QUERY_STRING', '')
request_method = os.environ.get('REQUEST_METHOD', '')

print("Content-Type: text/html\r")
print("\r")

print("<!DOCTYPE html>")
print("<html><head><title>CGI Test</title></head><body>")
print("<h1>CGI Script Test</h1>")
print("<h2>URL Parsing:</h2>")
print(f"<p><b>SCRIPT_NAME:</b> {script_name}</p>")
print(f"<p><b>PATH_INFO:</b> {path_info}</p>")
print(f"<p><b>QUERY_STRING:</b> {query_string}</p>")
print(f"<p><b>REQUEST_METHOD:</b> {request_method}</p>")

if path_info:
    parts = path_info.strip('/').split('/')
    print("<h2>PATH_INFO parsed:</h2>")
    for i, part in enumerate(parts):
        print(f"<p>Part {i}: {part}</p>")

if query_string:
    print("<h2>Query Parameters:</h2>")
    params = query_string.split('&')
    for param in params:
        if '=' in param:
            key, value = param.split('=', 1)
            print(f"<p><b>{key}:</b> {value}</p>")

print("</body></html>")