#!/bin/bash

# Tests de Transfer-Encoding: chunked sans CGI
# Envoie des POST sur la route racine avec contenu chunked

echo "=== Test 1: Simple POST chunked vers / ==="
{
    echo -ne "POST / HTTP/1.1\r\n"
    echo -ne "Host: localhost:8080\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1 : "Hello"
    echo -ne "5\r\n"
    echo -ne "Hello"
    echo -ne "\r\n"
    
    # Chunk 2 : " World"
    echo -ne "6\r\n"
    echo -ne " World"
    echo -ne "\r\n"
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8080

echo -e "\n=== Test 2: Trois petits chunks ==="
{
    echo -ne "POST / HTTP/1.1\r\n"
    echo -ne "Host: localhost:8080\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1
    echo -ne "3\r\n"
    echo -ne "Foo"
    echo -ne "\r\n"
    
    # Chunk 2
    echo -ne "3\r\n"
    echo -ne "Bar"
    echo -ne "\r\n"
    
    # Chunk 3
    echo -ne "3\r\n"
    echo -ne "Baz"
    echo -ne "\r\n"
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8080

echo -e "\n=== Test 3: Chunks avec données plus longues ==="
{
    echo -ne "POST / HTTP/1.1\r\n"
    echo -ne "Host: localhost:8080\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1
    echo -ne "1f\r\n"
    echo -ne "This is a longer chunk data"
    echo -ne "\r\n"
    
    # Chunk 2
    echo -ne "19\r\n"
    echo -ne "And another one here too"
    echo -ne "\r\n"
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8080

echo -e "\n=== Test 4: Chunks fragmentés (délais entre les chunks) ==="
{
    echo -ne "POST / HTTP/1.1\r\n"
    echo -ne "Host: localhost:8080\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1
    echo -ne "a\r\n"
    echo -ne "ChunkOne"
    echo -ne "\r\n"
    sleep 0.1
    
    # Chunk 2
    echo -ne "a\r\n"
    echo -ne "ChunkTwo"
    echo -ne "\r\n"
    sleep 0.1
    
    # Chunk 3
    echo -ne "b\r\n"
    echo -ne "ChunkThree"
    echo -ne "\r\n"
    sleep 0.1
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8080

echo -e "\n=== Test 5: Un seul petit chunk ==="
{
    echo -ne "POST / HTTP/1.1\r\n"
    echo -ne "Host: localhost:8080\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1
    echo -ne "4\r\n"
    echo -ne "Test"
    echo -ne "\r\n"
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8080

echo -e "\n=== Test 6: Chunk avec contenu spécial (symboles) ==="
{
    echo -ne "POST / HTTP/1.1\r\n"
    echo -ne "Host: localhost:8080\r\n"
    echo -ne "Transfer-Encoding: chunked\r\n"
    echo -ne "Connection: close\r\n"
    echo -ne "\r\n"
    
    # Chunk 1
    echo -ne "1b\r\n"
    echo -ne "param1=value&param2=test!"
    echo -ne "\r\n"
    
    # Chunk 2
    echo -ne "10\r\n"
    echo -ne "numbers=123&id=456"
    echo -ne "\r\n"
    
    # Chunk final
    echo -ne "0\r\n"
    echo -ne "\r\n"
    
} | nc -q 1 localhost 8080

echo -e "\n=== Tous les tests terminés ==="
