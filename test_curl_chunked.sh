#!/bin/bash

# Test simple Transfer-Encoding: chunked avec curl

echo "=== Test curl avec Transfer-Encoding: chunked ==="

curl -X POST \
  -H "Transfer-Encoding: chunked" \
  -d "client_name=Curl&guests=7&res_time=20:30" \
  -v http://localhost:8081/cgi-bin/reservation.php

echo -e "\n=== Test curl terminé ==="
