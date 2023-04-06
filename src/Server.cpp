/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:22:04 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/06 13:22:06 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(char **av)
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	opt = 1;
	addrlen = sizeof(sockaddr_in);
	port = atoi(av[1]);
	password = av[2];
	bzero(buffer, 1024);
}

Server::~Server()
{
	close(socket_fd);
}

int		Server::getSocket(void)
{
	return (socket_fd);
}

int		Server::getAccept(void)
{
	return (accept_fd);
}

char	*Server::getBuffer(void)
{
	return (buffer);
}

void	Server::setAccept(void)
{
	accept_fd = accept(
		socket_fd,
		reinterpret_cast<struct sockaddr*>(&address),
		reinterpret_cast<socklen_t*>(&addrlen)
	);
}

void	Server::setAddress(void)
{
	memset(reinterpret_cast<char*>(&address), 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);
}

sockaddr	*Server::getCastAddress(void)
{
	return (reinterpret_cast<struct sockaddr*>(&address));
}
