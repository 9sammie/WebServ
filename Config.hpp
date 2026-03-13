/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vakozhev <vakozhev@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 10:13:49 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/13 14:05:02 by vakozhev         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <string>
#include <vector>
#include <map>
#include <cstddef> //size_t

struct ListenConfig
{
	ListenConfig();
	std::string host;//si vide refuser, verifier a.b.c.d avec 4 octets 0...255
	int port; //valeurs a refuser <=0 et > 65535, non numerique
};

struct LocationConfig
{
	LocationConfig();
	std::string prefix; // /srcs, /files, /
	std::string root; 
	std::string index; 
	bool autoindex;
	std::vector<std::string> methods; //si non precise, j autorise GET seulement par defaut, a verifier des doublons..
	bool uploadAuthorised; // si cette location accepte upload, pour stocker body, mais pas pour CGI (ou dire si POST autorise et pas CGI) 
	bool cgiAuthorised; // si CGI est active sur cette location ?
	std::string cgiExt; //extenstion p ex .py besoin ?
	std::string cgiPath; //chemin de l executable pour lancer le script 
	bool hasRedirection; // si redirection est configuree
	int redirectCode; // 301 permanent 302 temporaire, 307 et 308 ??? dans quelle mesure c est important ?
	std::string redirectTarget; //new path ?
	bool hasMaxBodySize;
	size_t maxBodySize;
	bool hasKeepalive;
	int keepaliveTimeoutSec;
};

struct ServerConfig
{
	ServerConfig();
	bool hasKeepalive;
	int keepaliveTimeoutSec;
	std::vector<ListenConfig> listens; // au moins 1 dans ce container, En mémoire, ça donne :
										//listens.size() == 1
										//listens[0].host == "127.0.0.1"
										//listens[0].port == 8080
	std::string serverName;
	bool hasMaxBodySize; // ou je prends celui de hpp ?
	size_t maxBodySize;
	std::map<int, std::string> errors; // cle : code d erreur, 
										//valeur : le chemin de la page a afficher
	std::vector<LocationConfig> locations; // en memoire, cela donne :
											//locations[0].prefix = "/"
											//locations[1].prefix = "/srcs"
											//locations[2].prefix = "/uploads"
											//locations[3].prefix = "/files"
											//locations[4].prefix = "/cgi"
};

struct HttpConfig
{
	HttpConfig();
	int keepaliveTimeoutSec;
	size_t maxBodySize;
	std::map<int, std::string> errors;
	std::vector<ServerConfig> servers;
};

#endif
