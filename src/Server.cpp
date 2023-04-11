/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:22:04 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 15:38:43 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(char **av)
{
	/* int socket(int domain, int type, int protocol);
	AF_INET: IPv4 Internet protocols (man 7 ip)
	SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based
	bytestreams. An out-of-band data transmission mechanism may be supported.
	0: Normally only a single protocol exists to support a particular socket
	type within a given protocol family, in which case protocol can be 0.*/
	_master_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	_addrlen = sizeof(sockaddr_in);
	_opt = 1;
	_port = atoi(av[1]);
	_password = av[2];
	bzero(_buffer, 1024);
}

Server::~Server()
{
	close(_master_socket_fd);
}

int		Server::getMasterSocket(void)
{
	return (_master_socket_fd);
}

int		Server::getAccept(void)
{
	return (_accept_fd);
}

char	*Server::getBuffer(void)
{
	return (_buffer);
}

void	Server::setAccept(void)
{
	_accept_fd = accept(
		_master_socket_fd,
		reinterpret_cast<sockaddr*>(&_address),
		reinterpret_cast<socklen_t*>(&_addrlen)
	);
}

void	Server::setAddress(void)
{
	/* memset(): Init the struct sockaddr_in to 0
	AF_INET: IPv4 Internet protocols (man 7 ip)
	htonl(INADDR_ANY): converts the unsigned integer INADDR_ANY from host byte
	order to network byte order.
	htons(_port): converts the unsigned short integer _port from host byte 
	order to network byte order.*/
	memset(reinterpret_cast<char*>(&_address), 0, sizeof(sockaddr_in));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
}

int			*Server::getPtrOpt(void)
{
	return (&_opt);
}

sockaddr	*Server::getCastAddress(void)
{
	return (reinterpret_cast<sockaddr*>(&_address));
}
