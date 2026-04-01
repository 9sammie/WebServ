import subprocess
import os

# Couleurs pour le terminal
GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

def run_test(name, method, url, data=None, expected_code="200"):
    cmd = ["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", "-X", method, url]
    if data:
        cmd += ["-d", data]
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    actual_code = result.stdout.strip()
    
    if actual_code == expected_code:
        print(f"{GREEN}[PASS]{RESET} {name} (Status: {actual_code})")
        return True
    else:
        print(f"{RED}[FAIL]{RESET} {name} (Expected {expected_code}, got {actual_code})")
        return False

# --- CONFIGURATION DES TESTS ---
base_url = "http://localhost:8080"

tests = [
    # --- GET TESTS ---
    ("GET Home Page", "GET", f"{base_url}/", None, "200"),
    ("GET Inexistent File", "GET", f"{base_url}/nop.txt", None, "404"),
    ("GET Directory Listing", "GET", f"{base_url}/srcs/", None, "200"),
    
    # --- METHOD ALLOWED TESTS ---
    ("Method Not Allowed (DELETE on /)", "DELETE", f"{base_url}/", None, "405"),
    
    # --- POST & UPLOAD TESTS ---
    ("POST New File", "POST", f"{base_url}/uploads/test_upload.txt", "Hello World", "201"),
    # ("POST Too Large Body", "POST", f"{base_url}/uploads/big.txt", "A" * 2000000, "413"),
    
    # --- DELETE TESTS ---
    ("DELETE File", "DELETE", f"{base_url}/files/test_upload.txt", None, "204"),
    ("DELETE Non-existent", "DELETE", f"{base_url}/files/nothing.txt", None, "404"),
    
    # --- CGI TESTS ---
    ("CGI GET with Query", "GET", f"{base_url}/cgi/test.py?user=vako", None, "200"),
    ("CGI POST", "POST", f"{base_url}/cgi/test.py", "data=secret", "200"),
    
    # --- REDIRECTION ---
    ("Redirection 301", "GET", f"{base_url}/old-page", None, "301"),
]

# --- EXECUTION ---
print(f"--- Starting WebServ Tests on {base_url} ---")
passed = 0
for t in tests:
    if run_test(*t):
        passed += 1

print(f"\nResults: {passed}/{len(tests)} tests passed.")
