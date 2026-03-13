# Plan d'action pour la suite du projet WebServ

## 1. Mise à jour de la classe Client (Réseau)
- [] **Récupérer l'IP et le port dans `acceptNewConnection`** :
  - Dans `ServerManager::acceptNewConnection`, utiliser la structure `sockaddr_in address` retournée par `accept()`.
  - Convertir `address.sin_addr.s_addr` en string lisible avec `inet_ntoa()`.
  - Convertir `address.sin_port` en entier (host byte order) avec `ntohs()`.
- [] **Modifier la classe `Client`** :
  - Ajouter les champs membres privés pour stocker l'adresse IP (`std::string _remoteAddr`) et le port (`int _remotePort`).
  - Mettre à jour le constructeur de `Client` dans `Client.hpp` et `Client.cpp` pour prendre et initialiser ces nouvelles informations.
  - Ajouter des getters (`getRemoteAddr()` et `getRemotePort()`).

## 2. Refonte du chainage ServerManager / RequestHandler (Architecture)
- [] **Créer un utilitaire de recherche de ServerConfig** :
  - Créer une méthode dans `ServerManager` (ex: `const ServerConfig& getMatchingServer(int port, const std::string& hostHeader);`).
  - Cette méthode cherchera parmi la configuration globale celui qui "match" le fd client et le header "Host".
- [] **Modifier la signature de `RequestHandler`** :
  - Dans `RequestHandler.hpp` et `RequestHandler.cpp`, changer le constructeur : il ne doit plus prendre un `std::vector<ServerConfig>`, mais doit prendre uniquement une référence (`const ServerConfig&`).
- [] **Référer la bonne connexion dans `ServerManager::receivedRequest()`** :
  - Avant d'instancier un `RequestHandler`, parser/extraire le champ `Host` depuis la première partie de la requête contenue dans le `_requestBuffer` du `Client`.
  - Effectuer la recherche du `ServerConfig` avec la méthode créée.
  - **Coder la logique de la méthode `getMatchingServer`** :
    - Extraire la première ligne (et/ou l'en-tête "Host:") de la requête HTTP brute (contenue dans le buffer).
    - Récupérer le `serverName` demandé par le client depuis cet en-tête.
    - Itérer sur le tableau `_httpConfig.servers` et renvoyer le `ServerConfig` dont le `serverName` (et le port) correspond à la demande. S'il n'y a pas de match exact, prévoir une mécanique d'abandon ou de retour vers un serveur par défaut.
  - Instancier le `RequestHandler` avec cette configuration précise.

## 3. Mise à jour des informations du CGI
- [] Mettre à jour `DataCgi` pour inclure proprement `remoteAddr` et `remotePort` afin de pouvoir générer les variables d'environnement `REMOTE_ADDR` et `REMOTE_PORT`.
