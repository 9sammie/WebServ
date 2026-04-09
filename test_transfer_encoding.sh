#!/bin/bash

# Test Transfer-Encoding: chunked avec netcat
# Le format chunked : taille_hex\r\n données\r\n ... 0\r\n\r\n

echo "=== Test 1: Transfer-Encoding chunked avec reservation.php ==="

# Données à envoyer : client_name=Test&guests=2&res_time=18:00
# On va les envoyer en chunks

{
    # Headers avec Transfer-Encoding: chunked
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1 : "client_name="  (11 octets)
    # echo -ne "b\r\n"
    echo -ne "client_name="
    # echo -ne "\r\n"
    
    # Chunk 2 : "Test&guests=" (12 octets)
    # echo -ne "c\r\n"
    echo -ne "Test&guests="
    # echo -ne "\r\n"
    
    # Chunk 3 : "2&res_time=18:00" (15 octets)
    # echo -ne "f\r\n"
    echo -ne "2&res_time=18:00"
    # echo -ne "\r\n"
    
    # Chunk final
    echo -ne "0\r\n"
    # echo -ne "\r\n"
    
} | nc localhost 8081

echo -e "\n=== Test 2: Test avec curl ==="
curl -X POST \
  -H "Transfer-Encoding: chunked" \
  -d "client_name=Chunked&guests=3&res_time=20:00" \
  -v http://localhost:8081/cgi-bin/reservation.php

echo -e "\n=== Tests terminés ==="
