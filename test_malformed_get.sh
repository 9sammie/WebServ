#!/bin/bash

# Test GET requests malformés et méthodes interdites
# Tests pour découvrir des erreurs potentielles

PORT=8080
HOST="localhost"
RESULTS_FILE="test_malformed_results.txt"

echo "=== Tests GET Malformés et Méthodes Interdites ===" | tee "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 1: GET sans URI
echo "[TEST 1] GET sans URI (seulement GET HTTP/1.1)" | tee -a "$RESULTS_FILE"
(echo -ne "GET HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 2: GET sans HTTP version
echo "[TEST 2] GET sans HTTP version" | tee -a "$RESULTS_FILE"
(echo -ne "GET /\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 3: GET avec HTTP version invalide (HTTP/2)
echo "[TEST 3] GET avec HTTP/2" | tee -a "$RESULTS_FILE"
(echo -ne "GET / HTTP/2\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 4: GET avec HTTP/0.9
echo "[TEST 4] GET avec HTTP/0.9" | tee -a "$RESULTS_FILE"
(echo -ne "GET / HTTP/0.9\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 5: GET avec headers mal formés (pas de ':')
echo "[TEST 5] GET avec header mal formé (pas de ':')" | tee -a "$RESULTS_FILE"
(echo -ne "GET / HTTP/1.1\r\nHost $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 6: GET avec ligne vide manquante (header et body fusionnés)
echo "[TEST 6] GET sans ligne vide avant le body (header fusionné)" | tee -a "$RESULTS_FILE"
(echo -ne "GET / HTTP/1.1\r\nHost: $HOST:$PORT\r\nsome body content"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 7: GET avec chemin invalide (double slashes)
echo "[TEST 7] GET avec double slashes ///" | tee -a "$RESULTS_FILE"
(echo -ne "GET ///home.html HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 8: GET avec path traversal
echo "[TEST 8] GET avec path traversal (../)" | tee -a "$RESULTS_FILE"
(echo -ne "GET /../../etc/passwd HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 9: Method PUT (non supportée)
echo "[TEST 9] Method PUT (interdite)" | tee -a "$RESULTS_FILE"
(echo -ne "PUT / HTTP/1.1\r\nHost: $HOST:$PORT\r\nContent-Length: 4\r\n\r\ndata"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 10: Method DELETE (non supportée)
echo "[TEST 10] Method DELETE (interdite)" | tee -a "$RESULTS_FILE"
(echo -ne "DELETE / HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 11: Method PATCH (non supportée)
echo "[TEST 11] Method PATCH (interdite)" | tee -a "$RESULTS_FILE"
(echo -ne "PATCH / HTTP/1.1\r\nHost: $HOST:$PORT\r\nContent-Length: 4\r\n\r\ndata"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 12: Method TRACE
echo "[TEST 12] Method TRACE (interdite)" | tee -a "$RESULTS_FILE"
(echo -ne "TRACE / HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 13: Method CONNECT
echo "[TEST 13] Method CONNECT (interdite)" | tee -a "$RESULTS_FILE"
(echo -ne "CONNECT localhost:8080 HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 14: Méthode vide
echo "[TEST 14] Méthode vide" | tee -a "$RESULTS_FILE"
(echo -ne " / HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 15: GET en minuscules (non standard)
echo "[TEST 15] GET en minuscules (get)" | tee -a "$RESULTS_FILE"
(echo -ne "get / HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 16: GET avec URI très long
echo "[TEST 16] GET avec URI très long (1000+ caractères)" | tee -a "$RESULTS_FILE"
LONG_URI=$(printf 'a%.0s' {1..1000})
(echo -ne "GET /$LONG_URI HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 17: GET avec caractères spéciaux dans l'URI
echo "[TEST 17] GET avec caractères spéciaux dans l'URI" | tee -a "$RESULTS_FILE"
(echo -ne "GET /%00%01%02 HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Test 18: GET avec query string très complexe
echo "[TEST 18] GET avec query string complexe" | tee -a "$RESULTS_FILE"
(echo -ne "GET /?param1=value1&param2=value2&param3=value3&test=longvalue HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n"; sleep 0.5) | nc -w 1 $HOST $PORT 2>&1 | head -20 | tee -a "$RESULTS_FILE"
echo "---" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

echo "Tests terminés! Résultats sauvegardés dans $RESULTS_FILE"
