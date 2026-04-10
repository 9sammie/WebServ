#!/bin/bash

echo "=== Test 1: POST chunked simple - réservation basique en un chunk ==="
# Chunk: "client_name=Remy&guests=2&res_time=20%3A00" (43 octets = 0x2b)
<<<<<<< HEAD
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2a\r\nclient_name=Remy&guests=2&res_time=20%%3A00\r\n0\r\n\r\n") | nc localhost 8081
=======
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8081\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2a\r\nclient_name=Remy&guests=2&res_time=20%%3A00\r\n0\r\n\r\n") | nc localhost 8081
>>>>>>> maballetPathFix
echo -e "\n"

echo "=== Test 2: POST chunked avec deux chunks - séparation name/guests ==="
# Chunk 1: "client_name=Linguini&" (21 octets = 0x15)
# Chunk 2: "guests=1&res_time=19%3A30" (25 octets = 0x19)
<<<<<<< HEAD
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "15\r\nclient_name=Linguini&\r\n19\r\nguests=1&res_time=19%%3A30\r\n0\r\n\r\n") | nc localhost 8081
=======
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8081\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "15\r\nclient_name=Linguini&\r\n19\r\nguests=1&res_time=19%%3A30\r\n0\r\n\r\n") | nc localhost 8081
>>>>>>> maballetPathFix
echo -e "\n"

echo "=== Test 3: POST chunked avec trois chunks - fragmentation par paramètre ==="
# Chunk 1: "client_name=Colette" (19 octets = 0x13)
# Chunk 2: "&guests=1" (9 octets = 0x9)
# Chunk 3: "&res_time=21%3A15" (17 octets = 0x11)
<<<<<<< HEAD
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "13\r\nclient_name=Colette\r\n9\r\n&guests=1\r\n11\r\n&res_time=21%%3A15\r\n0\r\n\r\n") | nc localhost 8081
=======
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8081\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "13\r\nclient_name=Colette\r\n9\r\n&guests=1\r\n11\r\n&res_time=21%%3A15\r\n0\r\n\r\n") | nc localhost 8081
>>>>>>> maballetPathFix
echo -e "\n"

echo "=== Test 4: POST chunked avec quatre chunks - très fragmenté ==="
# Chunk 1: "client" (6 octets = 0x6)
# Chunk 2: "_name=Skinner&" (14 octets = 0xe)
# Chunk 3: "guests=8" (8 octets = 0x8)
# Chunk 4: "&res_time=18%3A45" (17 octets = 0x11)
<<<<<<< HEAD
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "6\r\nclient\r\ne\r\n_name=Skinner&\r\n8\r\nguests=8\r\n11\r\n&res_time=18%%3A45\r\n0\r\n\r\n") | nc localhost 8081
=======
(printf "POST /cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8081\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "6\r\nclient\r\ne\r\n_name=Skinner&\r\n8\r\nguests=8\r\n11\r\n&res_time=18%%3A45\r\n0\r\n\r\n") | nc localhost 8081
>>>>>>> maballetPathFix
echo -e "\n"