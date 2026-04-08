#!/bin/bash

# Test 4 isolé : Trois petits chunks avec délai (multiple paquets)

echo "=== Test 4: Trois petits chunks avec délai ==="
{
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1
    echo -ne "7\r\n"
    echo -ne "Charlie"
    echo -ne "\r\n"
    sleep 0.05
    
    # Chunk 2
    echo -ne "1\r\n"
    echo -ne "r"
    echo -ne "\r\n"
    sleep 0.05
    
    # Chunk 3
    echo -ne "c\r\n"
    echo -ne "guests=2&"
    echo -ne "\r\n"
    sleep 0.05
    
    # Chunk 4
    echo -ne "f\r\n"
    echo -ne "res_time=17:45"
    echo -ne "\r\n"
    sleep 0.05
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8081

echo -e "\n=== Test 4 terminé ==="
