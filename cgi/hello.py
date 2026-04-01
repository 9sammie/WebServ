#!/usr/bin/env python3
import os
import sys

# CGI response headers
print("Content-Type: text/plain")
print("")  # blank line mandatory between headers and body

# Print env vars set by CgiHandler
print("=== CGI Environment ===")
for key in ["REQUEST_METHOD", "QUERY_STRING", "SCRIPT_NAME",
            "GATEWAY_INTERFACE", "SERVER_PROTOCOL",
            "HTTP_HOST", "HTTP_USER_AGENT"]:
    print(f"{key} = {os.environ.get(key, '(not set)')}")

# For POST: read body from stdin
method = os.environ.get("REQUEST_METHOD", "")
if method == "POST":
    print("\n=== POST Body ===")
    body = sys.stdin.read()
    print(body)
