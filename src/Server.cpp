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

Server::Server(char **av)
{
	_socket_fd = 0;
	_port = 0;
	_password = av[2];
	_accept_fd = 0;
	_opt = 1;
	_max_clients = MAX_CLIENT;
	_clients = std::vector<Client *>();
	_channels = std::vector<Channel *>();
	bzero(_buffer, 1024);
	_die = false;
}

Server::~Server()
{
	for (std::vector<Client*>::iterator it = _clients.begin();
	it != _clients.end(); it++)
		delete (*it);
	for (std::vector<Channel*>::iterator it = _channels.begin();
	it != _channels.end(); it++)
		delete (*it);
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

void	Server::setDie(void)
{
	_die = true;
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

Client	*Server::getClientFromNick(std::string nick)
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it)->getNickname() == nick)
			return (*it);
	}
	return (NULL);
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

	std::stringstream			ss(command);
	std::string					item;
	std::vector<std::string>	tokens;

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
			user(tokens[i], client);
		else if (startwith("PING", tokens[i]))
			ping(client);
		else if (startwith("PASS", tokens[i]))
			pass(tokens[i], client);
		else if (startwith("MODE", tokens[i]))
			mode(tokens[i], client);
		else if (startwith("PRIVMSG", tokens[i]))
			privmsg(tokens[i], client);
		else if (startwith("JOIN", tokens[i]))
			join(tokens[i], client);
		else if (startwith("WHO", tokens[i]))
			who(tokens[i], client);
		else if (startwith("TOPIC", tokens[i]))
			topic(tokens[i], client);
		else if (startwith("PART", tokens[i]))
			part(tokens[i], client);
		else if (startwith("die", tokens[i]))// Ajouter le if (client == admin)
			setDie();
	}
}

void	Server::topic(std::string command, Client *client)
{
	std::stringstream			ss(command);
	std::string					item;
	std::vector<std::string>	tokens;

	command[command.size()] = '\0';
	while (std::getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 2)
	{
		sendToClient(":serverserver 461 TOPIC :Not enough parameters", client);
		return ;
	}
	tokens[1][tokens[1].size()] = '\0';
	tokens[1].erase(0, 1);
	Channel	*channel = getChannel(tokens[1]);
	if (channel == NULL)
	{
		sendToClient(":serverserver 403 " + client->getUsername() + " " + tokens[1] + " :No such channel", client);
		return ;
	}
	if (channel->getOperators().size() > 0 && !channel->isOperator(client))
	{
		sendToClient(":serverserver 482 " + client->getUsername() + " " + tokens[1] + " :You're not channel operator", client);
		return ;
	}
	if (tokens.size() == 2)
	{
		if (channel->getTopic() == "")
			sendToClient(":serverserver 331 " + client->getUsername() + " #" + tokens[1] + " No topic is set", client);
		else
			sendToClient(":serverserver 332 " + client->getUsername() + " #" + tokens[1] + " " + channel->getTopic(), client);
	}
	else
	{
		std::string	topic = "";
		for (size_t i = 2; i < tokens.size(); i++)
			topic.append(tokens[i] + " ");
		channel->setTopic(topic);
		sendToClient(":serverserver 332 " + client->getUsername() + " #" + tokens[1] + " " + channel->getTopic(), client);
	}
}

/**
 * @brief Sends a welcome message to the client.
 * 
 * @param client 
 */
void	Server::welcome(Client *client)
{
	std::cout << "WELCOME" << std::endl;
	std::string buffer = ":serverserver 001 ";
	buffer.append(client->getNickname());
	buffer.append(" :coucou\r\n");
	write(client->getSocket(), buffer.c_str(), buffer.size());
	client->setWelcomed(0);
}

std::string	Server::getPassword(void)
{
	return(_password);
}

Channel *Server::getChannel(std::string name) {
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it)->getName() == name)
			return (*it);
	}
	return NULL;
}

bool		Server::getDie(void)
{
	return (_die);
}

// TODO: Implement all the commands here :

/**
 * Changes the nickname of the client.
 * 
 * If the User has already been welcomed, sends NICK message to the client.
 * 
 * @param command 
 * @param client 
 */
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
	if (this->getClientFromNick(tokens[1]) != NULL)
	{
		sendToClient(":serverserver " + Errors::ERR_NICKNAMEINUSE + " * " + tokens[1] + " :Nickname is already in use.", client);
		return ;
	}
	client->setNickname(tokens[1]);
	if (client->isWelcomed() == 0)
		sendToClient(":" + old_nickname + " NICK :" + client->getNickname(), client);
}

void	Server::pass(std::string command, Client *client)
{
	command.erase(0, 5);
	if (command != this->getPassword())
		client->setPass(0);
	else
		client->setPass(1);
}

void	Server::user(std::string command, Client *client)
{
	command.erase(0, 5);
	std::string	name = command;
	name.erase(name.find(" "));
	client->setUsername(name);
	name = command;
	name.erase(name.find(" "));
	client->setHostname(name);
	name = command;
	name.erase(name.find(" "));
	client->setHost(name);
	name = command;
	name.erase(name.find(" :"));
	client->setRealName(name);
	client->setOk(1);
}

/**
 * @brief Respond to a ping from a client.
 * 
 * @param client 
 */
void	Server::ping(Client *client)
{
	std::string	buffer;
	buffer = ":serverserver PONG serverserver :" + client->getUsername() + "\r\n";
	sendToClient(buffer, client);
}

void	Server::who(std::string command, Client *client)
{
	std::stringstream			ss(command);
	std::string					item;
	std::vector<std::string>	tokens;

	command[command.size()] = '\0';
	while (std::getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() >= 2 and tokens[1].at(0) == '#')
	{
		tokens[1].erase(0, 1);
		Channel *channel = getChannel(tokens[1]);
		if (channel == NULL)
		{
			std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
			sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
			return ;
		}
		if (tokens.size() == 2)
		{
			std::vector<Client *> clients = channel->getClients();
			for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
			{
				std::string	buffer = ":serverserver 352 " + client->getUsername() + " " + channel->getName() + " " + (*it)->getUsername()
				+ " " + (*it)->getHostname() + " " + (*it)->getHost() + " " + (*it)->getUsername() + " H :0 " + (*it)->getRealName();
				sendToClient(buffer, client);
			}
			std::string buffer = ":serverserver 315 " + client->getUsername() + " " + channel->getName() + " :End of /WHO list.";
		}
		else
		{
			// TODO: Implement the mode command for channels
			std::cout << CYAN << "Mode command for channels not implemented yet" << ENDL;
			return ;
		}
	}

	// TODO: Implement the mode command for users
}

void	Server::mode(std::string command, Client *client)
{
	std::stringstream			ss(command);
	std::string					item;
	std::vector<std::string>	tokens;

	command[command.size()] = '\0';
	while (std::getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() >= 2 and startwith("#", tokens[1]))
	{
		tokens[1].erase(0, 1);
		Channel *channel = getChannel(tokens[1]);
		if (channel == NULL)
		{
			std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
			sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
			return ;
		}
		if (tokens.size() == 2)
		{
			std::string buffer = " +";
			if (channel->isInviteOnly())
				buffer += "i";
			if (channel->getTopic() != "No topic yet")
				buffer += "n";
			if (buffer.size() == 2)
				buffer = "";
			sendToClient(":serverserver 324 " + client->getNickname() + " #" + channel->getName() + " " + buffer, client);
			return ;
		}
		else
		{
			// TODO: Implement the mode command for channels
			std::cout << CYAN << "Mode command for channels not implemented yet" << ENDL;
			return ;
		}
	}

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

/**
 * @brief Checks if a channel exists.
 * 
 * @param channel_name 
 * @return true 
 * @return false 
 */
bool	Server::channelExists(std::string channel_name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == channel_name)
			return (true);
	}
	return (false);
}

/**
 * Allows a client to join a channel.
 * If the channel does not exist, it is created and the client is added to it's op list.
 * 
 * The client is added to the channel's client list.
 *  
 * @param command 
 * @param client 
 */
void	Server::join(std::string command, Client *client)
{
	std::string	channel_name;

	command.erase(0, 5);
	bool added = false;
	if (command[0] == '#')
	{
		channel_name = command;
		channel_name.erase(0, 1);
	}
	else
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	if (not channelExists(channel_name))
	{
		std::cout << "Channel " << channel_name << " does not exist, creating it." << std::endl;
		Channel *channel = new Channel(channel_name);
		addChannel(channel);
		channel->addClient(client);
		added = true;
		channel->addOperator(client);
	}

	if (getChannel(channel_name)->isBanned(client))
	{
		std::cout << RED << "Client " << client->getUsername() << " cannot join " << channel_name << " because banned." << ENDL;
		sendToClient(": serverserver " + Errors::ERR_BANNEDFROMCHAN + " * :Cannot join channel (+b)", client);
		return ;
	}
	if (getChannel(channel_name)->isInviteOnly() && not getChannel(channel_name)->isInvited(client))
	{
		std::cout << RED << "Client " << client->getUsername() << " cannot join " << channel_name << " because invite only." << ENDL;
		sendToClient(": serverserver " + Errors::ERR_INVITEONLYCHAN + " * :Cannot join channel (+i)", client);
		return ;
	}

	if (not added)
		getChannel(channel_name)->addClient(client);

	sendToClient(":serverserver 332 " + client->getNickname() + " " + channel_name + " :" + getChannel(channel_name)->getTopic(), client);
	
	std::string buffer;
	for (size_t i = 0; i < getChannel(channel_name)->getClients().size(); i++)
	{
		std::vector<Client *> op = getChannel(channel_name)->getOperators();
		if (std::find(op.begin(), op.end(), getChannel(channel_name)->getClients()[i]) != op.end())
			buffer += "@";
		buffer += getChannel(channel_name)->getClients()[i]->getNickname();
		if (i != getChannel(channel_name)->getClients().size() - 1)
			buffer += " ";
	}
	sendToClient(":serverserver 353 " + client->getNickname() + " = " + channel_name + " :" + buffer, client);
	sendToClient(":serverserver 366 " + client->getNickname() + " " + channel_name + " :End of /NAMES list.", client);
	
	for (size_t i = 0; i < getChannel(channel_name)->getClients().size(); i++)
	{
		if (getChannel(channel_name)->getClients()[i] != client)
			sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN #"
			+ channel_name, getChannel(channel_name)->getClients()[i]);
	}
}

void	Server::part(std::string command, Client *client)
{
	std::string	channel_name;

	command.erase(0, 5);
	if (command[0] == '#')
	{
		channel_name = command;
		channel_name.erase(0, 1);
	}
	else
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}
	if (not channelExists(channel_name))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}
	Channel *channel = getChannel(channel_name);
	for (size_t i = 0; i < channel->getClients().size(); i++)
	{
		if (channel->getClients()[i] == client)
		{
			channel->removeClient(client);
			sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PART #"
			+ channel_name + " :Leaving channel", client);
			if (channel->getClients().size() == 0)
			{
				removeChannel(channel);
				delete channel;
			}
			else
			{
				for (size_t i = 0; i < channel->getClients().size(); i++)
				{
					if (channel->getClients()[i] != client)
						sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PART #"
						+ channel_name + " :Leaving channel", channel->getClients()[i]);
				}
			}
			return ;
		}
	}
	sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
}

// void	Server::part(std::string command, Client *client)
// {
// 	std::cout << "PART" << std::endl;
// }

void	Server::privmsg(std::string command, Client *client)
{
	std::vector<std::string> 	tokens;
	std::stringstream 			ss(command);
	std::string					item;
	std::string					message;

	while (std::getline(ss, item, ' '))
		tokens.push_back(item);
	if(tokens[0][0] == '#')
		std::cout << "message to channel" << ENDL;
	else
	{
		Client	*target;

		std::cout << "get tokens " << tokens[1] << ENDL;
		target = getClientFromNick(tokens[1]);
		message = tokens[2];
		for (size_t i = 3; i < tokens.size(); i++)
		{
			message.append(" ");
			message.append(tokens[i]);
		}
		message.append("\r\n");
		if(target != NULL)
			sendToClient(":" + client->getNickname() + " PRIVMSG " + target->getNickname() + " " + message, target);
		else
			sendToClient(":serverserver 401 " + client->getUsername() +  " " + tokens[1] + " :No such nick/channel\r\n", client);
	}
}

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
