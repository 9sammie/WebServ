#!/usr/bin/env python3
import time

SLEEP_DURATION = 60 # sleep duration in seconds

print("Content-Type: text/plain\r\n")
print(f"Sleeping for {SLEEP_DURATION} seconds...")
time.sleep(SLEEP_DURATION)
print("Done!")
