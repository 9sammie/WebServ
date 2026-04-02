#!/usr/bin/env python3
import random
import os
import sys

# Path to the dishes directory relative to where you launch ./webserv
DISHES_DIR = "./html/site1/dishes/"

def get_random_dish():
    try:
        # We take ALL .html files in the 'dishes' folder
        files = [f for f in os.listdir(DISHES_DIR) if f.endswith('.html')]
        
        if not files:
            return "/menu.html"
            
        chosen_file = random.choice(files)
        # We return the URL path that the browser understands
        return f"/dishes/{chosen_file}"
    except FileNotFoundError:
        return "/menu.html"

# Get the URL
target_url = get_random_dish()

# --- THE MAGIC HEADER ---
# This tells the server/browser to redirect immediately
print(f"Location: {target_url}")
print("") # Mandatory empty line