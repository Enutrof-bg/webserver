#!/usr/bin/env python3
"""
CGI script for file upload handling
"""
import os
import sys
import cgi

print("Content-Type: text/html\r")
print("\r")

method = os.environ.get('REQUEST_METHOD', 'GET')
content_type = os.environ.get('CONTENT_TYPE', '')

print("""<!DOCTYPE html>
<html>
<head>
    <title>Upload Result</title>
    <style>
        body { font-family: Arial; padding: 20px; background: #1a1a2e; color: #eee; }
        .success { color: #00ff88; }
        .error { color: #ff4444; }
        .info { background: rgba(255,255,255,0.1); padding: 15px; border-radius: 8px; margin: 10px 0; }
        a { color: #00d9ff; }
    </style>
</head>
<body>
""")

if method == 'POST' and 'multipart/form-data' in content_type:
    form = cgi.FieldStorage()
    
    if 'file' in form:
        fileitem = form['file']
        if fileitem.filename:
            # Secure the filename
            filename = os.path.basename(fileitem.filename)
            filepath = f'/tmp/uploads/{filename}'
            
            # Create uploads directory
            os.makedirs('/tmp/uploads', exist_ok=True)
            
            # Save file
            with open(filepath, 'wb') as f:
                f.write(fileitem.file.read())
            
            print(f'<h1 class="success">✅ Upload Successful!</h1>')
            print(f'<div class="info">')
            print(f'<p><strong>Filename:</strong> {filename}</p>')
            print(f'<p><strong>Saved to:</strong> {filepath}</p>')
            print(f'<p><strong>Size:</strong> {os.path.getsize(filepath)} bytes</p>')
            print(f'</div>')
        else:
            print('<h1 class="error">❌ No file selected</h1>')
    else:
        print('<h1 class="error">❌ No file field in form</h1>')
else:
    print('<h1 class="error">❌ Invalid request</h1>')
    print(f'<p>Method: {method}</p>')
    print(f'<p>Content-Type: {content_type}</p>')

print("""
    <p><a href="/test_webserv/">← Back to tests</a></p>
</body>
</html>
""")
