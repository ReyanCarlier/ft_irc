/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:22:04 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 17:57:38 by nfelsemb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server()
{
	/* int socket(int domain, int type, int protocol);
	AF_INET: IPv4 Internet protocols (man 7 ip)
	SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based
	bytestreams. An out-of-band data transmission mechanism may be supported.
	0: Normally only a single protocol exists to support a particular socket
	type within a given protocol family, in which case protocol can be 0.*/
	_socket_fd = 0;
	_port = 0;
	_password = NULL;
	_accept_fd = 0;
	_opt = 1;
	_max_clients = MAX_CLIENT;
	_clients = std::list<Client>();
	_channels = std::list<Channel>();
	bzero(_buffer, 1024);
}

Server::~Server()
{
	close(_socket_fd);
}

int		Server::getSocketFd(void)
{
	return (_socket_fd);
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
		_socket_fd,
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

Client	*Server::getClient(int i)
{
	if (i == 0)
		return (&_clients.front());
	if ((size_t)i > _clients.size())
		return (NULL);
	std::list<Client>::iterator it = _clients.begin();
	std::advance(it, i - 1);
	return (&(*it));
}

sockaddr	*Server::getCastAddress(void)
{
	return (reinterpret_cast<sockaddr*>(&_address));
}

void Server::bind(int port) {
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;

	if (::bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw "Failed to bind socket.";
}

void Server::listen() {
	if (::listen(_socket_fd, _max_clients) < 0)
		throw "Failed to listen on socket.";
}

void Server::run() {
	_socket_fd = socket(2, 1, 0);
	if (_socket_fd == -1)
		throw "Failed to create socket.";
}

int Server::getPort() {
	return _port;
}

void Server::addClient(Client &client) {
	_clients.push_back(client);
}

void Server::removeClient(Client client) {
	for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->getSocket() == client.getSocket()) {
			_clients.erase(it);
			break;
		}
	}
}

void Server::addChannel(Channel channel) {
	_channels.push_back(channel);
}

void Server::removeChannel(Channel channel) {
	for (std::list<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if (it->getName() == channel.getName()) {
			_channels.erase(it);
			break;
		}
	}
}

std::list<Client> Server::getClients() {
	return _clients;
}

std::list<Channel> Server::getChannels() {
	return _channels;
}