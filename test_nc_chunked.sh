#!/bin/bash

# Test Transfer-Encoding: chunked avec netcat
# Format chunked : taille_hex\r\n données\r\n ... 0\r\n\r\n

echo "=== Test 1: Simple POST chunked avec nc ==="
{
    # Headers avec Transfer-Encoding: chunked (PAS de Content-Length)
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1 : "client_name="  (11 octets = b en hex)
    echo -ne "b\r\n"
    echo -ne "client_name="
    echo -ne "\r\n"
    
    # Chunk 2 : "Test&guests=" (12 octets = c en hex)
    echo -ne "c\r\n"
    echo -ne "Test&guests="
    echo -ne "\r\n"
    
    # Chunk 3 : "2&res_time=18:00" (15 octets = f en hex)
    echo -ne "f\r\n"
    echo -ne "2&res_time=18:00"
    echo -ne "\r\n"
    
    # Chunk final (size 0 + \r\n\r\n)
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8081

echo -e "\n=== Test 2: Test avec plus grand contenu en chunks ==="
{
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Une seule donnée
    DATA="client_name=Alice&guests=4&res_time=19:30"
    SIZE=$((${#DATA}))
    HEX=$(printf "%x" $SIZE)
    
    echo -ne "$HEX\r\n"
    echo -ne "$DATA"
    echo -ne "\r\n"
    
    # Petit délai pour simuler deux paquets TCP séparés
    sleep 0.1
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8081

echo -e "\n=== Test 3: Deux chunks avec délai (multiple paquets) ==="
{
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1
    echo -ne "15\r\n"
    echo -ne "client_name=Bob"
    echo -ne "\r\n"
    sleep 0.05
    
    # Chunk 2
    echo -ne "1d\r\n"
    echo -ne "&guests=5&res_time=21:00"
    echo -ne "\r\n"
    sleep 0.05
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8081

echo -e "\n=== Test 4: Trois petits chunks avec délai (multiple paquets) ==="
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

echo -e "\n=== Test 5: Un seul paquet (tout d'un coup) ==="
{
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    echo -ne "12\r\n"
    echo -ne "client_name="
    echo -ne "\r\n"
    echo -ne "7\r\nDiana"
    echo -ne "\r\n"
    echo -ne "1b\r\n"
    echo -ne "&guests=3&res_time=20:15"
    echo -ne "\r\n"
    echo -ne "0\r\n"
    echo -ne "\r\n"
} | nc -q 1 localhost 8081

echo -e "\n=== Test 6: Un seul paquet avec donnée plus longue (tout d'un coup) ==="
{
    echo -ne "POST /cgi-bin/reservation.php HTTP/1.1\r\n"
    echo -ne "Host: localhost:8081\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    DATA="client_name=Eve&guests=6&res_time=22:00"
    SIZE=$((${#DATA}))
    HEX=$(printf "%x" $SIZE)
    
    echo -ne "$HEX\r\n$DATA\r\n0\r\n\r\n"
} | nc -q 1 localhost 8081

echo -e "\n=== Tests terminés ==="
