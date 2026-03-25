/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maballet <maballet@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 10:08:33 by vakozhev          #+#    #+#             */
/*   Updated: 2026/03/19 15:38:24 by maballet         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

ListenConfig::ListenConfig()
	: host("0.0.0.0")
	, port(0)
{}

LocationConfig::LocationConfig()
	: prefix("")
	, root("")
	, alias("")
	, index("")
	, autoindex(false)
	, uploadAuthorised(false)
	, cgiAuthorised(false)
	, cgiExt("")
	, cgiPath("")
	, hasRedirection(false)
	, redirectCode(0)
	, redirectTarget("")
	, hasMaxBodySize(false)
	, maxBodySize(0)
	, hasKeepalive(false)
	, keepaliveTimeoutSec(0)
{
	methods.push_back("GET"); //GET si rien n est precise, a reflechir si pertinent, gestion doublon...
}

ServerConfig::ServerConfig()
	: hasKeepalive(false)
	, keepaliveTimeoutSec(0)
	, listens()
	, serverName("")
	, hasMaxBodySize(false)
	, maxBodySize(0)
	, errors()
	, root("")
	, index("")
	, locations()
{}

HttpConfig::HttpConfig()
	: hasKeepalive(true)
	, keepaliveTimeoutSec(75)
	, hasMaxBodySize(true)
	, maxBodySize(1048576)
	, errors()
	, servers()
{}
