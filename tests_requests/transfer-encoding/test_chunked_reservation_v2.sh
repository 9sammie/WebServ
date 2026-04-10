#!/bin/bash

# Tests Transfer-Encoding: chunked avec POST sur le CGI reservation
# S'inspire de test_chunked_random_plate.sh
# reservation.php : traite les réservations (client_name, guests, res_time)
# Format chunked : taille_hex\r\n données\r\n ... 0\r\n\r\n

echo "=== Test 1: POST chunked simple - réservation basique en un chunk ==="
# Chunk: "client_name=Remy&guests=2&res_time=20%3A00" (43 octets = 0x2b)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2b\r\nclient_name=Remy&guests=2&res_time=20%%3A00\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 2: POST chunked avec deux chunks - séparation name/guests ==="
# Chunk 1: "client_name=Linguini&" (20 octets = 0x14)
# Chunk 2: "guests=1&res_time=19%3A30" (25 octets = 0x19)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "14\r\nclient_name=Linguini&\r\n19\r\nguests=1&res_time=19%%3A30\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 3: POST chunked avec trois chunks - fragmentation par paramètre ==="
# Chunk 1: "client_name=Colette" (19 octets = 0x13)
# Chunk 2: "&guests=1" (9 octets = 0x9)
# Chunk 3: "&res_time=21%3A15" (17 octets = 0x11)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "13\r\nclient_name=Colette\r\n9\r\n&guests=1\r\n11\r\n&res_time=21%%3A15\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 4: POST chunked avec quatre chunks - très fragmenté ==="
# Chunk 1: "client" (6 octets = 0x6)
# Chunk 2: "_name=Skinner&" (13 octets = 0xd)
# Chunk 3: "guests=8" (8 octets = 0x8)
# Chunk 4: "&res_time=18%3A45" (17 octets = 0x11)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "6\r\nclient\r\nd\r\n_name=Skinner&\r\n8\r\nguests=8\r\n11\r\n&res_time=18%%3A45\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 5: POST chunked avec chunks de tailles variées ==="
# Chunk 1: "cli" (3 octets = 0x3)
# Chunk 2: "ent_name=Ego&guests=" (20 octets = 0x14)
# Chunk 3: "1" (1 octet = 0x1)
# Chunk 4: "&res_time=22%3A00" (17 octets = 0x11)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "3\r\ncli\r\n14\r\nent_name=Ego&guests=\r\n1\r\n1\r\n11\r\n&res_time=22%%3A00\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 6: POST chunked avec réservation avec nom spécial ==="
# Chunk: "client_name=Gusteau%20(ghost)&guests=1&res_time=20%3A30" (56 octets = 0x38)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "38\r\nclient_name=Gusteau%%20(ghost)&guests=1&res_time=20%%3A30\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 7: POST chunked avec délai simulant paquets TCP séparés ==="
{
    printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
    printf "a\r\nclient_na\r\n"
    sleep 0.1
    printf "12\r\nme=Emile&guests=10\r\n"
    sleep 0.1
    printf "11\r\n&res_time=17%%3A00\r\n"
    sleep 0.1
    printf "0\r\n\r\n"
} | nc localhost 8080
echo -e "\n"

echo "=== Test 8: POST chunked avec heure invalide (devrait échouer) ==="
# Chunk: "client_name=Anton&guests=1&res_time=08%3A00" (44 octets = 0x2c)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "2c\r\nclient_name=Anton&guests=1&res_time=08%%3A00\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 9: POST chunked avec beaucoup de convives ==="
# Chunk 1: "client_name=Django&" (18 octets = 0x12)
# Chunk 2: "guests=100&res_time=" (20 octets = 0x14)
# Chunk 3: "12%3A00" (7 octets = 0x7)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "12\r\nclient_name=Django&\r\n14\r\nguests=100&res_time=\r\n7\r\n12%%3A00\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Test 10: POST chunked avec nom très long fragmenté ==="
# Chunk 1: "client_name=LeGrandChefDeLaCuisine" (34 octets = 0x22)
# Chunk 2: "&guests=5&" (10 octets = 0xa)
# Chunk 3: "res_time=15%3A45" (16 octets = 0x10)
(printf "POST /site2/cgi-bin/reservation.php HTTP/1.1\r\nHost: localhost:8080\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"; printf "22\r\nclient_name=LeGrandChefDeLaCuisine\r\na\r\n&guests=5&\r\n10\r\nres_time=15%%3A45\r\n0\r\n\r\n") | nc localhost 8080
echo -e "\n"

echo "=== Tous les tests reservation (v2) avec Transfer-Encoding terminés ==="
