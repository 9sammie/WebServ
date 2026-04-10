#!/bin/bash

# Tests Transfer-Encoding: chunked avec POST sur le CGI random-plate
# random-plate.py : redirige aléatoirement vers un plat
# Format chunked : taille_hex\r\n données\r\n ... 0\r\n\r\n

echo "=== Test 1: GET sur random-plate (pas de body requis) ==="
(printf "GET /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nConnection: close\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 2: POST chunked simple sur random-plate ==="
# Chunk 1: "seed=42" (7 octets = 0x7)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "7\r\nseed=42\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 3: POST chunked avec deux chunks ==="
# Chunk 1: "seed=" (5 octets = 0x5)
# Chunk 2: "12345" (5 octets = 0x5)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "5\r\nseed=\r\n5\r\n12345\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 4: POST chunked avec trois chunks ==="
# Chunk 1: "plate" (5 octets = 0x5)
# Chunk 2: "_type=" (6 octets = 0x6)
# Chunk 3: "pizza" (5 octets = 0x5)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "5\r\nplate\r\n6\r\n_type=\r\n5\r\npizza\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 5: POST chunked avec chunks de tailles variées ==="
# Chunk 1: "ref" (3 octets = 0x3)
# Chunk 2: "123456789" (9 octets = 0x9)
# Chunk 3: "ABC" (3 octets = 0x3)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "3\r\nref\r\n9\r\n123456789\r\n3\r\nABC\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 6: POST chunked avec un chunk moyen ==="
# Chunk: "category=mainCourse" (19 octets = 0x13)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "13\r\ncategory=mainCourse\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 7: POST chunked avec données urlencoded en chunks ==="
# Chunk 1: "user" (4 octets = 0x4)
# Chunk 2: "=test&" (6 octets = 0x6)
# Chunk 3: "time=dinner" (11 octets = 0xb)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "4\r\nuser\r\n6\r\n=test&\r\nb\r\ntime=dinner\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 8: POST chunked avec délai simulant paquets TCP séparés ==="
{
    printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
    printf "4\r\nid=1\r\n"
    sleep 0.1
    printf "5\r\n23456\r\n"
    sleep 0.1
    printf "0\r\n\r\n"
} | nc localhost 8080
echo -e "\n"

echo "=== Test 9: POST chunked avec paramètres multiples fragmentés ==="
# Chunk 1: "q=" (2 octets = 0x2)
# Chunk 2: "randomPlate&type=" (17 octets = 0x11)
# Chunk 3: "weekly" (6 octets = 0x6)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2\r\nq=\r\n11\r\nrandomPlate&type=\r\n6\r\nweekly\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 10: POST chunked avec très long chunk ==="
# Chunk: "reference=PLATE123456789ABCDEFGHIJKLMNOPQRST" (44 octets = 0x2c)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2c\r\nreference=PLATE123456789ABCDEFGHIJKLMNOPQRST\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Tous les tests random-plate avec Transfer-Encoding terminés ==="
