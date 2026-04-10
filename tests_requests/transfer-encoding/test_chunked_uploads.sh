#!/bin/bash

# Tests Transfer-Encoding: chunked avec POST sur /uploads
# Format chunked : taille_hex\r\n données\r\n ... 0\r\n\r\n

echo "=== Test 1: POST avec deux chunks simples ==="
(printf "POST /uploads/site1/test1.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "5\r\nHello\r\n6\r\n World\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 2: POST avec trois chunks ==="
(printf "POST /uploads/site1/test2.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "3\r\nFoo\r\n3\r\nBar\r\n3\r\nBaz\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 3: POST avec chunk unique plus long ==="
(printf "POST /uploads/site1/test3.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "1b\r\nThis is a longer chunk data\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 4: POST avec données spéciales (symboles et nombres) ==="
(printf "POST /uploads/site1/test4.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "19\r\nparam1=value&param2=test!\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 5: POST avec plusieurs chunks moyens ==="
(printf "POST /uploads/site1/test5.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "10\r\n0123456789ABCDEF\r\n10\r\nGHIJKLMNOPQRSTUV\r\n10\r\nWXYZabcdefghijkl\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 6: POST avec un seul petit chunk ==="
(printf "POST /uploads/site1/test6.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "4\r\nTest\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 7: POST avec données de formulaire en chunks ==="
(printf "POST /uploads/site1/test7.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "b\r\nname=Alice&\r\n6\r\nage=25\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 8: POST avec chunks avec délai (simulation paquets TCP séparés) ==="
{
    printf "POST /uploads/site1/test8.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
    printf "9\r\nChunk One\r\n"
    sleep 0.1
    printf "9\r\nChunk Two\r\n"
    sleep 0.1
    printf "b\r\nChunk Three\r\n"
    sleep 0.1
    printf "0\r\n\r\n"
} | nc localhost 8080
echo -e "\n"

echo "=== Test 9: POST avec contenu JSON en chunks ==="
(printf "POST /uploads/site1/test9.json HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n"; printf "18\r\n{\"name\":\"John\",\"age\":30}\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 10: POST avec très long contenu en chunks ==="
(printf "POST /uploads/site1/test10.txEst HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "18\r\nThis is line number one.\r\n18\r\nThis is line number two.\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Tous les tests terminés ==="
