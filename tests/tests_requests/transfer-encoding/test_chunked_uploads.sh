#!/bin/bash

# Tests Transfer-Encoding: chunked with POST on /uploads
# Chunked format: hex_size\r\n data\r\n ... 0\r\n\r\n

echo "=== Test 1: POST with two simple chunks ==="
(printf "POST /uploads/site1/test1.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "5\r\nHello\r\n6\r\n World\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 2: POST with three chunks ==="
(printf "POST /uploads/site1/test2.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "3\r\nFoo\r\n3\r\nBar\r\n3\r\nBaz\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 3: POST with single longer chunk ==="
(printf "POST /uploads/site1/test3.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "1b\r\nThis is a longer chunk data\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 4: POST with special data (symbols and numbers) ==="
(printf "POST /uploads/site1/test4.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "19\r\nparam1=value&param2=test!\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 5: POST with multiple medium chunks ==="
(printf "POST /uploads/site1/test5.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "10\r\n0123456789ABCDEF\r\n10\r\nGHIJKLMNOPQRSTUV\r\n10\r\nWXYZabcdefghijkl\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 6: POST with single small chunk ==="
(printf "POST /uploads/site1/test6.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "4\r\nTest\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 7: POST with form data in chunks ==="
(printf "POST /uploads/site1/test7.txt HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "b\r\nname=Alice&\r\n6\r\nage=25\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 8: POST with chunks with delay (simulation of separate TCP packets) ==="
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

echo "=== Test 9: POST with JSON content in chunks ==="
(printf "POST /uploads/site1/test9.json HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n"; printf "18\r\n{\"name\":\"John\",\"age\":30}\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 10: POST with very long content in chunks ==="
(printf "POST /uploads/site1/test10.txEst HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "18\r\nThis is line number one.\r\n18\r\nThis is line number two.\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== All tests completed ==="
