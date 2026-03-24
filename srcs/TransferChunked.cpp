/*

    Quoi faire :
    Ajouter a client une variable taille chunk initialisee a -1 pour pouvoir savoir quand on est au dernier chunk de taille 0
    Ajouter a client _chunkBuffer Pas forcement utile, on peut utiliser _requestBuffer et stocker petit a petit dedans
    nouvelle variable transfer chunked true or false pour optimiser

    if (_transferChunked = false){
        if find Transfer-chunked && !content-length
        {
            _transferChunked = true
    
        }
    }
    if (transferChunked = true){
        function that will check sizeChunk, if size == 0
        getChunkSize()
        else
            getChunkData
            {
            
                ici, on ercupere une string de taille correspondante, on verifie que juste apres on a bien \r\n
                sinon, erreur 400, on fermera le client
                on substr cette string et on la stocke dans _requestBuffer
                on clean le debut de rawBuffer en virant les premiers octets de taille sizeChunk
                Cas specifique du dernier chunk, il faudra calculer la taille pour les trailer possibles avant le \r\n
            }
    }









*/