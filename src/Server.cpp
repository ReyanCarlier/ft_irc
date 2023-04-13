#include <Server.hpp>

bool	startwith(std::string prefix, std::string str)
{
	int	i = 0;

	if (prefix.length() > str.length())
		return (false);
	while (prefix[i])
	{
		if (prefix[i] != str[i])
			return (false);
		i++;
	}
	return (true);
}

Server::Server()
{
	_socket_fd = 0;
	_port = 0;
	_password = NULL;
	_accept_fd = 0;
	_opt = 1;
	_max_clients = MAX_CLIENT;
	_clients = std::vector<Client *>();
	_channels = std::vector<Channel *>();
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

/**
 * @brief Returns the biggest fd from the clients
 * 
 * @param readfds 
 * @param writefds 
 * @return int 
 */
int		Server::getHighestFd(fd_set *readfds, fd_set *writefds)
{
	int		highest_fd = _socket_fd;

	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		FD_SET((*it)->getSocket(), readfds);
		FD_SET((*it)->getSocket(), writefds);
		if ((*it)->getSocket() > highest_fd)
			highest_fd = (*it)->getSocket();
	}
	return (highest_fd);
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
	memset(reinterpret_cast<char*>(&_address), 0, sizeof(sockaddr_in));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
}

int			*Server::getPtrOpt(void)
{
	return (&_opt);
}

Client	*Server::getClient(int fd)
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it)->getSocket() == fd)
			return (*it);
	}
	throw "Client not found.";
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
	if (setsockopt(_socket_fd, SOL_SOCKET,
	SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof(int)))
		throw "Failed to set options on sockets.";
}

int Server::getPort() {
	return _port;
}

void Server::addClient(Client *client) {
	_clients.push_back(client);
}

void Server::removeClient(Client *client) {
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if ((*it)->getSocket() == client->getSocket()) {
			_clients.erase(it);
			break;
		}
	}
}

void Server::addChannel(Channel *channel) {
	_channels.push_back(channel);
}

void Server::removeChannel(Channel *channel) {
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it)->getName() == channel->getName()) {
			_channels.erase(it);
			break;
		}
	}
}

std::vector<Client *> Server::getClients() {
	return _clients;
}

std::vector<Channel *> Server::getChannels() {
	return _channels;
}

void	Server::sendToClient(std::string message, Client *client)
{
	std::cout << GREEN << "[SERVER => CLIENT (" << client->getSocket() << ")]\n" << YELLOW << message << ENDL;
	std::cout << "----------------------------------------" << std::endl;
	message.append("\r\n");
	send(client->getSocket(), message.c_str(), message.length(), 0);
}

void	Server::commandHandler(std::string command, Client *client)
{
	std::cout << CYAN << "[CLIENT (" << client->getSocket() << ") => SERVER]\n" << YELLOW << command << ENDL;
	std::cout << "----------------------------------------" << std::endl;
	std::stringstream ss(command);
	std::string		item;
	std::vector<std::string> tokens;

	if (client->isWelcomed())
		return welcome(command, client);

	while (std::getline(ss, item, '\n'))
		tokens.push_back(item);

	for (size_t i = 0; i < tokens.size(); i++)
		if (tokens[i].at(tokens[i].size() - 1) == '\r')
			tokens[i].erase(tokens[i].size() - 1);

	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (startwith("NICK", tokens[i]))
			nick(tokens[i], client);
		else if (startwith("USER", tokens[i]))
			std::cout << "USER" << std::endl;
		else if (startwith("PING", tokens[i]))
			ping(client);
		else if (startwith("PASS", tokens[i]))
			std::cout << "PASS" << std::endl;
		else if (startwith("MODE", tokens[i]))
			mode(tokens[i], client);
		// Ajouter les autres commandes ici
	}
}

void	Server::welcome(std::string command, Client *client)
{
	std::string buffer;

	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Welcoming " << client->getUsername() << std::endl;
	std::cout << "----------------------------------------" << std::endl;

	command[command.size() - 1] = '\0';
	client->setMessage(command);
	buffer = client->getMessage();
	if (startwith("CAP LS\r\n", buffer))
		buffer.erase(0, 8);
	if (startwith("NICK ", buffer))
	{
		std::string nickname = buffer;
		nickname.erase(0, 5);
		nickname.erase(nickname.find("\r\n"));
		client->setNickname(nickname);
		buffer.erase(0, 5 + nickname.size() + 2);
	}
	if (startwith("USER ", buffer))
	{
		std::string tmp;

		buffer.erase(0,5);
		tmp = buffer;
		tmp.erase(tmp.find(" "));
		client->setUsername(tmp);
		client->setNickname(tmp);
		
		tmp = buffer;
		tmp.erase(tmp.find(" "));
		client->setHostname(tmp);

		tmp = buffer;
		tmp.erase(tmp.find(" "));
		client->setHost(tmp);

		tmp = buffer;
		tmp.erase(tmp.find(" "));
		client->setRealName(tmp);
		client->setOk(1);
	}

	buffer = ":serverserver 001 " + client->getUsername() + " :coucou\r\n";
	write(client->getSocket(), buffer.c_str(), buffer.size());
	client->setWelcomed(0);
}

// TODO: Implement all the commands here :

void	Server::nick(std::string command, Client *client)
{
	std::stringstream 			ss(command);
	std::string					item;
	std::vector<std::string> 	tokens;
	std::string					old_nickname;

	while (std::getline(ss, item, ' '))
		tokens.push_back(item);

	if (tokens.size() < 2)
	{
		std::cout << RED << "Invalid command sent by " << client->getNickname() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NONICKNAMEGIVEN + " * :No nickname given", client);
		return ;
	}
	if (tokens.size() > 2)
	{
		std::cout << RED << "Invalid command sent by " << client->getNickname() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_ERRONEUSNICKNAME + " * :Erroneous nickname", client);
		return ;
	}
	old_nickname = client->getNickname();
	client->setNickname(tokens[1]);
	client->setUsername(tokens[1]);
	if (client->isWelcomed() == 0)
		sendToClient(":" + old_nickname + " NICK :" + client->getNickname(), client);
}

// void	Server::user(std::string command, Client *client)
// {
// 	std::cout << "USER" << std::endl;
// }

void	Server::ping(Client *client)
{
	std::string buffer;

	buffer = ":serverserver PONG serverserver :" + client->getUsername() + "\r\n";
	sendToClient(buffer, client);
}

void	Server::mode(std::string command, Client *client)
{
	std::stringstream ss(command);
	std::string		item;
	std::vector<std::string> tokens;

	command[command.size()] = '\0';
	while (std::getline(ss, item, ' '))
		tokens.push_back(item);
	
	if (tokens.size() < 3)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
		return ;
	}
	if (tokens[2] == "+i")
		client->setMode(1);
	else if (tokens[2] == "-i")
		client->setMode(0);
	else
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_UMODEUNKNOWNFLAG + " * :Unknown MODE flag", client);
		return ;
	}

	sendToClient(":serverserver MODE " + client->getUsername() + " " + tokens[2], client);
}

// void	Server::pass(std::string command, Client *client)
// {
// 	std::cout << "PASS" << std::endl;
// }

// void	Server::join(std::string command, Client *client)
// {
// 	std::cout << "JOIN" << std::endl;
// }

// void	Server::part(std::string command, Client *client)
// {
// 	std::cout << "PART" << std::endl;
// }

// void	Server::privmsg(std::string command, Client *client)
// {
// 	std::cout << "PRIVMSG" << std::endl;
// }

// void	Server::quit(std::string command, Client *client)
// {
// 	std::cout << "QUIT" << std::endl;
// }

// void	Server::who(std::string command, Client *client)
// {
// 	std::cout << "WHO" << std::endl;
// }

// void	Server::whois(std::string command, Client *client)
// {
// 	std::cout << "WHOIS" << std::endl;
// }

// void	Server::whowas(std::string command, Client *client)
// {
// 	std::cout << "WHOWAS" << std::endl;
// }

// void	Server::list(std::string command, Client *client)
// {
// 	std::cout << "LIST" << std::endl;
// }

