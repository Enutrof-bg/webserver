#!/usr/bin/env python3
"""
CGI script to display all CGI environment variables
"""
import os
import sys

print("Content-Type: text/html\r")
print("\r")

print("""<!DOCTYPE html>
<html>
<head>
    <title>CGI Environment</title>
    <style>
        body { 
            font-family: 'Courier New', monospace; 
            background: #0a0a0a; 
            color: #0f0; 
            padding: 20px;
        }
        h1 { color: #0f0; border-bottom: 1px solid #0f0; padding-bottom: 10px; }
        table { border-collapse: collapse; width: 100%; }
        td, th { 
            border: 1px solid #0f0; 
            padding: 8px; 
            text-align: left; 
        }
        th { background: #1a1a1a; }
        .cgi-var { color: #0ff; }
        .http-var { color: #ff0; }
        a { color: #0f0; }
    </style>
</head>
<body>
    <h1>🖥️ CGI Environment Variables</h1>
    <table>
        <tr><th>Variable</th><th>Value</th></tr>
""")

# Sort environment variables
env_vars = sorted(os.environ.items())

for key, value in env_vars:
    css_class = 'cgi-var' if key.startswith(('REQUEST', 'CONTENT', 'SCRIPT', 'PATH', 'QUERY', 'SERVER', 'REMOTE')) else 'http-var' if key.startswith('HTTP_') else ''
    # Escape HTML
    value = value.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')
    if len(value) > 100:
        value = value[:100] + '...'
    print(f'        <tr><td class="{css_class}">{key}</td><td>{value}</td></tr>')

print("""    </table>
    <p><a href="/test_webserv/">← Back to tests</a></p>
</body>
</html>""")
