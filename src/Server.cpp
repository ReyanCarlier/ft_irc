/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:22:04 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/06 16:27:57 by nfelsemb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(char **av)
{
	_master_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	_addrlen = sizeof(sockaddr_in);
	_port = atoi(av[1]);
	_password = av[2];
	bzero(_buffer, 1024);
	_max_client = 42;
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

int		Server::getMaxClient(void)
{
	return (_max_client);
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
	memset(reinterpret_cast<char*>(&_address), 0, sizeof(sockaddr_in));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
}

sockaddr	*Server::getCastAddress(void)
{
	return (reinterpret_cast<sockaddr*>(&_address));
}
