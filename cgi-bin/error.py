#!/usr/bin/env python3
"""
CGI script that generates an error - for testing 500 errors
"""
import sys

# This will cause the CGI to fail
print("Content-Type: text/html\r")
print("\r")

# Force an error
raise Exception("Intentional CGI error for testing")
