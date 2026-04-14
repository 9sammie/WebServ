#!/usr/bin/env python3
import time

# ===== MODIFIE CETTE VALEUR =====
SLEEP_DURATION = 60 # durée du sleep en secondes
# ================================

print("Content-Type: text/plain\r\n")
print(f"Sleeping for {SLEEP_DURATION} seconds...")
time.sleep(SLEEP_DURATION)
print("Done!")
