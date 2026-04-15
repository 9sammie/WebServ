#!/bin/bash

# Tests Transfer-Encoding: chunked with POST on the random-plate CGI
# random-plate.py: randomly redirects to a dish
# Chunked format: hex_size\r\n data\r\n ... 0\r\n\r\n

echo "=== Test 1: GET on random-plate (no body required) ==="
(printf "GET /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nConnection: close\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 2: Simple POST chunked on random-plate ==="
# Chunk 1: "seed=42" (7 bytes = 0x7)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "7\r\nseed=42\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 3: POST chunked with two chunks ==="
# Chunk 1: "seed=" (5 bytes = 0x5)
# Chunk 2: "12345" (5 bytes = 0x5)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "5\r\nseed=\r\n5\r\n12345\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 4: POST chunked with three chunks ==="
# Chunk 1: "plate" (5 bytes = 0x5)
# Chunk 2: "_type=" (6 bytes = 0x6)
# Chunk 3: "pizza" (5 bytes = 0x5)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "5\r\nplate\r\n6\r\n_type=\r\n5\r\npizza\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 5: POST chunked with chunks of various sizes ==="
# Chunk 1: "ref" (3 bytes = 0x3)
# Chunk 2: "123456789" (9 bytes = 0x9)
# Chunk 3: "ABC" (3 bytes = 0x3)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "3\r\nref\r\n9\r\n123456789\r\n3\r\nABC\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 6: POST chunked with a medium chunk ==="
# Chunk: "category=mainCourse" (19 bytes = 0x13)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "13\r\ncategory=mainCourse\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 7: POST chunked with urlencoded data in chunks ==="
# Chunk 1: "user" (4 bytes = 0x4)
# Chunk 2: "=test&" (6 bytes = 0x6)
# Chunk 3: "time=dinner" (11 bytes = 0xb)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "4\r\nuser\r\n6\r\n=test&\r\nb\r\ntime=dinner\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 8: POST chunked with delay simulating separate TCP packets ==="
{
    printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
    printf "4\r\nid=1\r\n"
    sleep 0.1
    printf "5\r\n23456\r\n"
    sleep 0.1
    printf "0\r\n\r\n"
} | nc localhost 8080
echo -e "\n"

echo "=== Test 9: POST chunked with multiple fragmented parameters ==="
# Chunk 1: "q=" (2 bytes = 0x2)
# Chunk 2: "randomPlate&type=" (17 bytes = 0x11)
# Chunk 3: "weekly" (6 bytes = 0x6)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2\r\nq=\r\n11\r\nrandomPlate&type=\r\n6\r\nweekly\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 10: POST chunked with very long chunk ==="
# Chunk: "reference=PLATE123456789ABCDEFGHIJKLMNOPQRST" (44 bytes = 0x2c)
(printf "POST /cgi-bin/random-plate.py HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2c\r\nreference=PLATE123456789ABCDEFGHIJKLMNOPQRST\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== All random-plate tests with Transfer-Encoding completed ==="
