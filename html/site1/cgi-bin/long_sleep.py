#!/usr/bin/env python3

import time
import sys

# On affiche juste un en-tête valide
print("Content-type: text/plain\n")
sys.stdout.flush()

try:
    for i in range(10):
        print(f"Sleeping... {i+1}/10")
        sys.stdout.flush()
        time.sleep(1)
        
    print("\nFin du sleep long !")
except Exception as e:
    pass