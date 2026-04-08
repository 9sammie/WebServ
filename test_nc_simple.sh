#!/bin/bash

# Test simple Transfer-Encoding: chunked avec netcat
# Un seul chunk simple pour debug

echo "=== Test simple Transfer-Encoding: chunked ==="
{
    # Headers
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Un seul chunk simple
    # "test" = 4 octets = 4 en hex
    echo -ne "4\r\n"
    echo -ne "test"
    echo -ne "\r\n"
    
    # Chunk final (size 0)
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8081

echo -e "\n=== Test terminé ==="
