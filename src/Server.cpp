/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:23:40 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/27 13:23:43 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

bool				Server::startwith(string prefix, string str) {
	int	i = 0;

	if (prefix.length() > str.length())
		return (false);
	while (prefix[i]) {
		if (prefix[i] != str[i])
			return (false);
		i++;
	}
	return (true);
}

Server::Server(char **av) {
	_socket_fd = 0;
	_port = 0;
	_password = av[2];
	_accept_fd = 0;
	_opt = 1;
	_max_clients = MAX_CLIENT;
	_clientinqueue = 0;
	_clients = vector<Client *>();
	_channels = vector<Channel *>();
	bzero(_buffer, BUFFER_SIZE);
	_die = false;
	_passwordadmin = OPER_PASSWORD;
}

Server::~Server() {
	vector<Client*>		clients = getClients();
	vector<Channel*>	channels = getChannels();

	for (vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
		delete *it;
	for (vector<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
		delete *it;
	close(_socket_fd);
}

int					Server::getSocketFd(void) {
	return (_socket_fd);
}

int					Server::getAccept(void) {
	return (_accept_fd);
}

char*				Server::getBuffer(void) {
	return (_buffer);
}

/**
 * @brief Returns the biggest fd from the clients
 * 
 * @param readfds 
 * @param writefds 
 * @return int 
 */
int					Server::getHighestFd(fd_set *readfds, fd_set *writefds) {
	int	highest_fd = _socket_fd;

	for (CL_IT = _clients.begin(); it != _clients.end(); it++) {
		FD_SET((*it)->getSocket(), readfds);
		FD_SET((*it)->getSocket(), writefds);
		if ((*it)->getSocket() > highest_fd)
			highest_fd = (*it)->getSocket();
	}
	return (highest_fd);
}

void				Server::setAccept(void) {
	_accept_fd = accept(
		_socket_fd,
		reinterpret_cast<sockaddr*>(&_address),
		reinterpret_cast<socklen_t*>(&_addrlen)
	);
}

void				Server::setAddress(void) {
	memset(reinterpret_cast<char*>(&_address), 0, sizeof(sockaddr_in));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
}

int*				Server::getPtrOpt(void) {
	return (&_opt);
}

Client*				Server::getClient(int fd) {
	for (CL_IT = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it)->getSocket() == fd)
			return (*it);
	}
	throw "Client not found.";
}

Client*				Server::getClientFromNick(string nick) {
	for (CL_IT = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it)->getNickname() == nick)
			return (*it);
	}
	return (NULL);
}

sockaddr*			Server::getCastAddress(void) {
	return (reinterpret_cast<sockaddr*>(&_address));
}

void				Server::bind(int port) {
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	_port = port;
	if (::bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw "Failed to bind socket.";
}

void				Server::listen() {
	if (::listen(_socket_fd, _max_clients) < 0)
		throw "Failed to listen on socket.";
}

void				Server::run() {
	_socket_fd = socket(2, 1, 0);
	if (_socket_fd == -1)
		throw "Failed to create socket.";
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt,
		sizeof(int))
	)
		throw "Failed to set options on sockets.";
}

int					Server::getPort() {
	return _port;
}

void				Server::addClient(Client *client) {
	_clients.push_back(client);
}

void				Server::removeClient(Client *client) {
	for (CL_IT = _clients.begin(); it != _clients.end(); it++) {
		if ((*it)->getSocket() == client->getSocket()) {
			Client *client = *it;
			_clients.erase(it);
			delete client;
			break ;
		}
	}
}

void				Server::addChannel(Channel *channel) {
	_channels.push_back(channel);
}

void				Server::removeChannel(Channel *channel) {
	for (CH_IT = _channels.begin(); it != _channels.end(); it++) {
		if ((*it)->getName() == channel->getName()) {
			Channel *channel = *it;
			_channels.erase(it);
			delete channel;
			break ;
		}
	}
}

vector<Client*>		Server::getClients() {
	return _clients;
}

vector<Channel*>	Server::getChannels() {
	return _channels;
}

void				Server::sendToClient(string message, Client *client) {
	cout << GREEN << "[SERVER => CLIENT (" << client->getSocket() << ")]\n"
	<< YELLOW << message << RESET << LINE << endl;
	message.append("\r\n");
	send(client->getSocket(), message.c_str(), message.length(), MSG_NOSIGNAL);
}

void				Server::commandHandler(string command, Client *client) {
	cout << CYAN << "[CLIENT (" << client->getSocket() << ") => SERVER]\n"
	<< YELLOW << command << RESET << LINE << endl;
	if (command.size() == 0)
		return ;
	if (command.at(0) == '\n')
		return ;
	if (command.at(command.size() - 1) != '\n') {
		if (client->getBuffer().size() > 0)
			command = client->getBuffer() + command;
		client->setBuffer(command);
		return ;
	} else if (client->getBuffer().size() > 0) {
		command = client->getBuffer() + command;
		client->setBuffer("");
	}

	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	while (getline(ss, item, '\n'))
		tokens.push_back(item);
	for (size_t i = 0; i < tokens.size(); i++)
		if (tokens[i].at(tokens[i].size() - 1) == '\r')
			tokens[i].erase(tokens[i].size() - 1);
	for (size_t i = 0; i < tokens.size(); i++) {
		if (startwith("NICK ", tokens[i]))
			nick(tokens[i], client);
		else if (startwith("USER ", tokens[i]))
			user(tokens[i], client);
		else if (startwith("PING ", tokens[i]))
			ping(client);
		else if (startwith("PASS ", tokens[i]))
			pass(tokens[i], client);
		else if (startwith("MODE ", tokens[i]))
			mode(tokens[i], client);
		else if (startwith("PRIVMSG ", tokens[i]))
			privmsg(tokens[i], client);
		else if (startwith("JOIN ", tokens[i]))
			join(tokens[i], client);
		else if (startwith("WHO ", tokens[i]))
			who(tokens[i], client);
		else if (startwith("TOPIC ", tokens[i]))
			topic(tokens[i], client);
		else if (startwith("PART ", tokens[i]))
			part(tokens[i], client);
		else if (tokens[i] == "die")
			die(client);
		else if (startwith("KICK ", tokens[i]))
			kick(tokens[i], client);
		else if (startwith("BAN ", tokens[i]))
			ban(tokens[i], client);
		else if (startwith("UNBAN ", tokens[i]))
			unban(tokens[i], client);
		else if (tokens[i] == "LIST")
			list(tokens[i], client);
		else if (startwith("QUIT ", tokens[i]))
			quit(tokens[i], client);
		else if (startwith("INVITE ", tokens[i]))
			invite(tokens[i], client);
		else if (startwith("OPER ", tokens[i]))
			oper(tokens[i], client);
		else if (startwith("kill ", tokens[i]))
			kill(tokens[i], client);
		else if (tokens[i] == "time")
			time(tokens[i], client);
		else if (tokens[i] == "version")
			version(tokens[i], client);
		else if (startwith("NAMES ", tokens[i]))
			names(tokens[i], client);
	}
}

/**
 * @brief Sends a welcome message to the client.
 * 
 * @param client 
 */
void				Server::welcome(Client *client) {
	sendToClient(":serverserver 001 " + client->getNickname() + " :Ceci est un message d'accueil.", client);
	client->setWelcomed(0);
}

string				Server::getPassword(void) {
	return(_password);
}

Channel*			Server::getChannel(string name) {
	if (_channels.size() == 0)
		return NULL;

	vector<Channel*>	channels = _channels;

	for (CH_IT = channels.begin(); it != channels.end(); it++) {
		if ((*it != NULL) && (*it)->getName() == name)
			return (*it);
	}
	return NULL;
}

bool				Server::getDie(void) {
	return (_die);
}

/**
 * @brief Checks if a channel exists.
 * 
 * @param channel_name 
 * @return true 
 * @return false 
 */
bool				Server::channelExists(string channel_name) {
	for (size_t i = 0; i < _channels.size(); i++) {
		if (_channels[i]->getName() == channel_name)
			return (true);
	}
	return (false);
}

void				Server::setpassadmin(string pass) {
	_passwordadmin = pass;
}

string				Server::getpassadmin(void) {
	return (_passwordadmin);
}

void				Server::addClientQueue(void) {
	_clientinqueue++;
}

void				Server::removeClientQueue(void) {
	_clientinqueue--;
}

int					Server::getClientQueue(void) {
	return (_clientinqueue);
}
