#include "SESSIONMANAGER.HPP"

Cookie_t::Cookie_t()
    : id("")
    , expDate(0)
    , lastAccess(0)
    , createdAt(0)
{}

std::string	SessionManager::generateSessionId() const
{
    //static pour ne pas recreer ce tab a chaque appel
    static const char alphabet[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    std::string id;
    id.reserve(SESSION_ID_LEN);
    for (int i = 0; i < SESSION_ID_LEN; i++)
    {
        //on choisit une position aleatoire dans l alphabet,
        // on prend le car a cette position
        //on l ajoute a la fin de id
        int index = std::rand() % (sizeof(alphabet) - 1); //62
        id = id + alphabet[index];
    }
    return id;
}

//est ce que id est expire ? 
bool SessionManager::isExpired(const Cookie_t& cookie) const
{
    std::time_t now = std::time(NULL);
    if (now >= cookie.expDate)
        return true;
    return false;
}


