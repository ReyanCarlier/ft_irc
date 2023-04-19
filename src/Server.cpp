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
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		delete (*it);
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
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
		else if (startwith("KICK", tokens[i]))
			kick(tokens[i], client);
		else if (startwith("BAN", tokens[i]))
			ban(tokens[i], client);
		else if (startwith("UNBAN", tokens[i]))
			unban(tokens[i], client);
		else if (startwith("LIST", tokens[i]))
			list(tokens[i], client);
		else if (startwith("QUIT", tokens[i]))
			quit(tokens[i], client);
		else if (startwith("INVITE", tokens[i]))
			invite(tokens[i], client);
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
		{
			if (i == 2 and tokens[2][0] == ':')
				tokens[2].erase(0, 1);
		}
		channel->setTopic(topic);
		std::vector<Client *> clients = channel->getClients();
		for (size_t i = 0; i < clients.size(); i++)
			sendToClient(":serverserver 332 " + client->getUsername() + " #" + tokens[1] + " :" + channel->getTopic(), clients[i]);
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
	
	// Send NICK message to all the clients in the same channels as the client.
	std::vector<Channel *> channels;
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		if ((*it)->isInChannel(client)) {
			channels.push_back((*it));
		}
	}

	if (channels.size() > 0)
	{
		for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++) {
			std::vector<Client *> clients = (*it)->getClients();
			for (std::vector<Client *>::iterator it2 = clients.begin(); it2 != clients.end(); it2++) {
				if ((*it2) != client)
					sendToClient(":" + old_nickname + " NICK :" + client->getNickname(), (*it2));
			}
		}
	}
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

/**
 * @brief Reaction to "WHO" command.
 * It sends a list of users on a channel.
 * 
 * @param command 
 * @param client 
 */
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
				std::string buffer = ":serverserver 352 " + client->getNickname() + " " + channel->getName() + " " + (*it)->getUsername() + " " + (*it)->getHostname() + " " + (*it)->getHost() + " " + (*it)->getUsername() + " H :0 " + (*it)->getRealName();
				sendToClient(buffer, client);
			}
			std::string buffer = ":serverserver 315 " + client->getNickname() + " " + channel->getName() + " :End of /WHO list.";
			sendToClient(buffer, client);
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
			if (channel->isInChannel(client) == false)
			{
				std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
				sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
				return ;
			}

			if (channel->isOperator(client) == false)
			{
				std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
				sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You're not channel operator", client);
				return ;
			}

			// BAN COMMAND
			if (tokens[2] == "+b")
			{
				if (tokens.size() < 4)
				{
					std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
					return ;
				}
				Client *target = NULL;
				std::string username = tokens[3];
				username.erase(0, 3);
				username = username.substr(0, username.find('@'));
				for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
				{
					if ((*it)->getUsername() == username and (*it)->getNickname() != client->getNickname())
					{
						target = (*it);
						break ;
					}
				}
				if (target == NULL)
				{
					std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(": serverserver " + Errors::ERR_NOSUCHNICK + " * :No such nick", client);
					return ;
				}

				std::string reason = "";
				if (tokens.size() > 4)
				{
					for (size_t i = 4; i < tokens.size(); i++)
						reason += tokens[i] + " ";
					reason = reason.substr(0, reason.size() - 1);
				}
				else
					reason = "Banned by " + client->getUsername();

				std::vector<Client *> clients = channel->getClients();
				for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
				{
					sendToClient(":serverserver MODE #" + channel->getName() + " +b !" + target->getNickname() + "@*", (*it));
					if ((*it)->getNickname() != target->getNickname())
						sendToClient(":serverserver KICK #" + channel->getName() + " " + target->getNickname() + " :" + reason, (*it));
				}
				part("PART #" + channel->getName() + " :" + reason, target);
				channel->addBanned(target);
				who("WHO #" + channel->getName(), client);
				return ;
			}
			// UNBAN COMMAND
			else if (tokens[2] == "-b")
			{
				if (tokens.size() < 4)
				{
					std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
					return ;
				}

				Client *target = NULL;
				std::vector<Client *> banned = channel->getBanned();
				std::string nickname = tokens[3];
				nickname.erase(0, 1);
				nickname = nickname.substr(0, nickname.find('@'));

				for (std::vector<Client *>::iterator it = banned.begin(); it != banned.end(); it++)
				{
					if ((*it)->getNickname() == nickname)
					{
						target = (*it);
						break ;
					}
				}

				if (target == NULL)
				{
					std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(": serverserver " + Errors::ERR_NOSUCHNICK + " * :Target isn't in blacklist of #" + channel->getName(), client);
					return ;
				}

				channel->unbanClient(target);
				std::vector<Client *> clients = channel->getClients();
				for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
					sendToClient(":serverserver MODE #" + channel->getName() + " -b !" + target->getNickname() + "@*", (*it));
				return ;
			}
			// SET INVITE ONLY
			else if (tokens[2] == "+i")
			{
				if (channel->isInviteOnly() == true)
				{
					std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :Channel is already invite only", client);
					return ;
				}
				channel->setInviteOnly(true);
				std::vector<Client *> clients = channel->getClients();
				for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
					sendToClient(":serverserver MODE #" + channel->getName() + " +i", (*it));
				return ;
			}
			else if (tokens[2] == "-i")
			{
				if (channel->isInviteOnly() == false)
				{
					std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :Channel is not invite only", client);
					return ;
				}
				channel->setInviteOnly(false);
				std::vector<Client *> clients = channel->getClients();
				for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
					sendToClient(":serverserver MODE #" + channel->getName() + " -i", (*it));
			}
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

	Channel *channel_to_join = getChannel(channel_name);
	if (not channel_to_join)
	{
		channel_to_join = new Channel(channel_name);
		addChannel(channel_to_join);
		channel_to_join->addClient(client);
		added = true;
		channel_to_join->addOperator(client);
	}

	if (channel_to_join->isBanned(client))
	{
		std::cout << RED << "Client " << client->getUsername() << " cannot join " << channel_name << " because banned." << ENDL;
		sendToClient(":serverserver " + Errors::ERR_BANNEDFROMCHAN + " * :Cannot join channel " + channel_name + " because you're banned.", client);
		return ;
	}

	if (channel_to_join->isInviteOnly() && not channel_to_join->isInvited(client))
	{
		std::cout << RED << "Client " << client->getUsername() << " cannot join " << channel_name << " because invite only." << ENDL;
		sendToClient(": serverserver " + Errors::ERR_INVITEONLYCHAN + " * :Cannot join channel (+i)", client);
		return ;
	}

	if (not added)
		getChannel(channel_name)->addClient(client);

	sendToClient(":serverserver 332 " + client->getNickname() + " " + channel_name + " :" + channel_to_join->getTopic(), client);
	
	std::string buffer;
	for (size_t i = 0; i < channel_to_join->getClients().size(); i++)
	{
		std::vector<Client *> op = channel_to_join->getOperators();
		if (std::find(op.begin(), op.end(), channel_to_join->getClients()[i]) != op.end())
			buffer += "@";
		buffer += channel_to_join->getClients()[i]->getNickname();
		if (i != channel_to_join->getClients().size() - 1)
			buffer += " ";
	}
	sendToClient(":serverserver 353 " + client->getNickname() + " = " + channel_name + " :" + buffer, client);
	sendToClient(":serverserver 366 " + client->getNickname() + " " + channel_name + " :End of /NAMES list.", client);
	
	for (size_t i = 0; i < channel_to_join->getClients().size(); i++)
	{
		sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN #" + channel_name, channel_to_join->getClients()[i]);
	}
}

/**
 * @brief Allows a client to leave a channel.
 * 
 * @param command 
 * @param client 
 */
void	Server::part(std::string command, Client *client)
{
	std::stringstream			ss(command);
	std::string					item;
	std::vector<std::string>	tokens;
	bool						removed_channel = false;

	command[command.size()] = '\0';
	while (std::getline(ss, item, ' '))
		tokens.push_back(item);

	std::string channel_name;

	if (tokens.size() == 1)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
		return ;
	}

	channel_name = tokens[1].erase(0, 1);

	Channel *channel = getChannel(channel_name);
	if (not channel)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	if (not channel->isInChannel(client))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
		return ;
	}

	for (size_t i = 0; i < channel->getClients().size(); i++)
	{
		if (channel->getClients()[i] == client)
		{
			if (channel->isOperator(client))
				channel->removeOperator(client);

			if (channel->isInvited(client))
				channel->removeInvited(client);

			for (size_t i = 0; i < channel->getClients().size(); i++)
				sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PART #" + channel_name + " :Leaving channel", channel->getClients()[i]);

			if (channel->getClients().size() == 1)
			{
				removeChannel(channel);
				removed_channel = true;
			}
			else
				channel->removeClient(client);
		}
	}
	if (removed_channel)
		delete channel;
}

void	Server::privmsg(std::string command, Client *client)
{
	std::vector<std::string> 	tokens;
	std::stringstream 			ss(command);
	std::string					item;
	std::string					message;

	while (std::getline(ss, item, ' '))
		tokens.push_back(item);

	if (tokens.size() < 3)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
		return ;
	}

	if(tokens[1][0] == '#')
	{
		Channel *channel = getChannel(tokens[1].substr(1, tokens[1].size() - 1));
		if (not channel)
		{
			sendToClient(":serverserver 403 " + client->getUsername() + " " + tokens[1] + " :No such channel\r\n", client);
			return ;
		}

		message = tokens[2];
		if (message[0] == ':')
			message.erase(0, 1);
		for (size_t i = 3; i < tokens.size(); i++)
		{
			message.append(" ");
			message.append(tokens[i]);
		}

		if (not channel->isInChannel(client))
		{
			std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
			sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
			return ;
		}

		if (channel->isMuted(client))
		{
			std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
			sendToClient(": serverserver " + Errors::ERR_CANNOTSENDTOCHAN + " * :Cannot speak in this channel, you're muted.", client);
			return ;
		}

		for (size_t i = 0; i < channel->getClients().size(); i++)
		{
			if (channel->getClients()[i] != client)
				sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PRIVMSG " + tokens[1] + " :" + message, channel->getClients()[i]);
		}
	}
	else
	{
		Client	*target;

		target = getClientFromNick(tokens[1]);
		if (not target)
			return sendToClient(":serverserver 401 " + client->getUsername() +  " " + tokens[1] + " :No user with this nickname.", client);

		message = tokens[2];
		for (size_t i = 3; i < tokens.size(); i++)
		{
			message.append(" ");
			message.append(tokens[i]);
		}
		sendToClient(":" + client->getNickname() + " PRIVMSG " + target->getNickname() + " " + message, target);	
	}
}

/**
 * @brief Kick a client from a channel. He can rejoin it if he wants.
 * 
 * @param command 
 * @param client 
 */
void	Server::kick(std::string command, Client *client)
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

	if (tokens[1][0] != '#')
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	tokens[1].erase(0, 1);

	Channel *channel = getChannel(tokens[1]);
	if (not channel)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	if (not channel->isInChannel(client))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
		return ;
	}

	if (not channel->isOperator(client))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You're not an operator in that channel", client);
		return ;
	}

	Client *client_to_kick = getClientFromNick(tokens[2]);
	if (not client_to_kick)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User doesn't exist", client);
		return ;
	}

	if (not channel->isInChannel(client_to_kick))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User is not in that channel", client);
		return ;
	}

	if (channel->isOperator(client_to_kick))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You can't kick an operator", client);
		return ;
	}

	std::string reason = "No reason.";
	if (tokens.size() > 3)
	{
		tokens[3].erase(0, 1);
		if (tokens[3].size() > 0)
			for (size_t i = 3; i < tokens.size(); i++)
				reason += tokens[i] + " ";
	}

	if (reason == "No reason.")
		reason = "Kicked by " + client->getNickname() + ".";
	part("PART #" + channel->getName() + " :" + reason, client_to_kick);

	for (size_t i = 0; i < channel->getClients().size(); i++)
		sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " KICK #" + channel->getName() + " " + client_to_kick->getNickname() + " :" + reason, channel->getClients()[i]);

	sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " KICK #" + channel->getName() + " " + client_to_kick->getNickname() + " :" + reason, client_to_kick);
}

/**
 * @brief Ban a user from a channel. He can't rejoin it unless he's unbanned.
 * 
 * @param command 
 * @param client 
 */
void	Server::ban(std::string command, Client *client)
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

	if (tokens[1][0] != '#')
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	tokens[1].erase(0, 1);
	
	Channel *channel = getChannel(tokens[1]);
	if (not channel)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	if (not channel->isInChannel(client))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
		return ;
	}

	if (not channel->isOperator(client))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You're not an operator in that channel", client);
		return ;
	}

	Client *client_to_ban = getClientFromNick(tokens[2]);
	if (not client_to_ban)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User doesn't exist", client);
		return ;
	}

	if (not channel->isInChannel(client_to_ban))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User is not in that channel", client);
		return ;
	}

	if (channel->isOperator(client_to_ban))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You can't ban an operator", client);
		return ;
	}

	std::string reason = "No reason.";
	if (tokens.size() > 3)
	{
		tokens[3].erase(0, 1);
		if (tokens[3].size() > 0)
			for (size_t i = 3; i < tokens.size(); i++)
				reason += tokens[i] + " ";
	}

	if (reason == "No reason.")
		reason = "Banned by " + client->getNickname() + ".";
	
	std::vector<Client *> clients = channel->getClients();

	for (size_t i = 0; i < clients.size(); i++)
		sendToClient(":serverserver MODE #" + channel->getName() + " +b !" + client_to_ban->getNickname() + "@*", clients[i]);
	
	kick("KICK #" + channel->getName() + " " + client_to_ban->getNickname() + " :" + reason, client);
	channel->addBanned(client_to_ban);
}

/**
 * @brief Mute a user in a channel.
 * 
 * @param command 
 * @param client 
 */
// void	Server::mute(std::string command, Client *client)
// {
// 	std::stringstream			ss(command);
// 	std::string					item;
// 	std::vector<std::string>	tokens;

// 	command[command.size()] = '\0';
// 	while (std::getline(ss, item, ' '))
// 		tokens.push_back(item);

// 	if (tokens.size() < 3)
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
// 		return ;
// 	}

// 	if (tokens[1][0] != '#')
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
// 		return ;
// 	}

// 	tokens[1].erase(0, 1);

// 	Channel *channel = getChannel(tokens[1]);
// 	if (not channel)
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
// 		return ;
// 	}

// 	if (not channel->isInChannel(client))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
// 		return ;
// 	}

// 	if (not channel->isOperator(client))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You're not an operator in that channel", client);
// 		return ;
// 	}

// 	Client *client_to_mute = getClientFromNick(tokens[2]);
// 	if (not client_to_mute)
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User doesn't exist", client);
// 		return ;
// 	}

// 	if (not channel->isInChannel(client_to_mute))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User is not in that channel", client);
// 		return ;
// 	}

// 	if (channel->isOperator(client_to_mute))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You can't mute an operator", client);
// 		return ;
// 	}

// 	channel->addMuted(client_to_mute);

// 	std::string reason = "No reason.";
// 	if (tokens.size() > 3)
// 	{
// 		reason = "";
// 		for (size_t i = 3; i < tokens.size(); i++)
// 			reason += tokens[i] + " ";
// 	}

// 	for (size_t i = 0; i < channel->getClients().size(); i++)
// 		sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " MUTE #" + channel->getName() + " +m " + client_to_mute->getNickname() + " :" + reason, channel->getClients()[i]);
// }

/**
 * @brief Unmute a user in a channel.
 * 
 * @param command 
 * @param client 
 */
// void	Server::unmute(std::string command, Client *client)
// {
// 	std::stringstream			ss(command);
// 	std::string					item;
// 	std::vector<std::string>	tokens;

// 	command[command.size()] = '\0';
// 	while (std::getline(ss, item, ' '))
// 		tokens.push_back(item);

// 	if (tokens.size() < 3)
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
// 		return ;
// 	}

// 	if (tokens[1][0] != '#')
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
// 		return ;
// 	}

// 	tokens[1].erase(0, 1);

// 	Channel *channel = getChannel(tokens[1]);
// 	if (not channel)
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
// 		return ;
// 	}

// 	if (not channel->isInChannel(client))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
// 		return ;
// 	}

// 	if (not channel->isOperator(client))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You're not an operator in that channel", client);
// 		return ;
// 	}

// 	Client *client_to_unmute = getClientFromNick(tokens[2]);
// 	if (not client_to_unmute)
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User doesn't exist", client);
// 		return ;
// 	}

// 	if (not channel->isInChannel(client_to_unmute))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User is not in that channel", client);
// 		return ;
// 	}

// 	if (channel->isOperator(client_to_unmute))
// 	{
// 		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
// 		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You can't unmute an operator", client);
// 		return ;
// 	}

// 	std::string reason = "No reason.";
// 	if (tokens.size() > 3)
// 	{
// 		reason = "";
// 		for (size_t i = 3; i < tokens.size(); i++)
// 			reason += tokens[i] + " ";
// 	}

// 	channel->unmuteClient(client_to_unmute);

// 	for (size_t i = 0; i < channel->getClients().size(); i++)
// 		sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " MUTE #" + channel->getName() + " -m " + client_to_unmute->getNickname() + " :" + reason, channel->getClients()[i]);
// }

/**
 * @brief Unban a user from a channel.
 * 
 * @param command 
 * @param client 
 */
void	Server::unban(std::string command, Client *client)
{
	std::stringstream			ss(command);
	std::string					item;
	std::vector<std::string>	tokens;

	command[command.size()] = '\0';
	while (std::getline(ss, item, ' '))
		tokens.push_back(item);

	if (tokens.size() < 3)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
		return ;
	}

	if (tokens[1][0] != '#')
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	tokens[1].erase(0, 1);

	Channel *channel = getChannel(tokens[1]);
	if (not channel)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	if (not channel->isInChannel(client))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
		return ;
	}

	if (not channel->isOperator(client))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You're not an operator in that channel", client);
		return ;
	}

	Client *client_to_unban = getClientFromNick(tokens[2]);
	if (not client_to_unban)
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User doesn't exist", client);
		return ;
	}

	if (not channel->isInChannel(client_to_unban))
	{
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User is not in that channel", client);
		return ;
	}

	channel->unbanClient(client_to_unban);

	for (size_t i = 0; i < channel->getClients().size(); i++)
		sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " MODE #" + channel->getName() + " -b " + client_to_unban->getNickname(), channel->getClients()[i]);
}

void	Server::list(std::string command, Client *client) {
	std::string	str;

	if (command.find('#') == std::string::npos) {
		for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++) {
			std::stringstream ss;

			ss << '#' << (*it)->getName() << ' ' << (*it)->getClients().size() << ' ' << (*it)->getTopic();
			str = ss.str();
			sendToClient(str, client);
		}
	} else {
		std::string	channel = command.substr(command.find('#') + 1);

		for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++) {
			if (channel != (*it)->getTopic())
				break ;

			std::stringstream ss;
			
			ss << '#' << (*it)->getName() << ' ' << (*it)->getClients().size() << ' ' << (*it)->getTopic();
			str = ss.str();
			sendToClient(str, client);
		}
	}
}

/**
 * @brief Exit the server properly. The Client is removed from all channels he's in.
 * 
 * @param command 
 * @param client 
 */
void	Server::quit(std::string command, Client *client) {
	std::string	reason;

	reason = command.substr(command.find(':') + 1);
	
	for (size_t i = 0; i < _channels.size(); i++) {
		Channel *channel = _channels.at(i);
		if (channel->isBanned(client))
			channel->unbanClient(client);
		if (channel->isMuted(client))
			channel->unmuteClient(client);
		if (channel->isInChannel(client))
			part("PART #" + channel->getName() + " :" + reason, client);
	}
	std::cerr << RED << "Client " << client->getSocket() << " disconnected. Reason : " << reason << ENDL;
	close(client->getSocket());
	removeClient(client);
	delete client;
}

/**
 * @brief Invite a Client to a Channel.
 * 
 */
void	Server::invite(std::string command, Client *client) {
	std::stringstream			ss(command);
	std::string					item;
	std::vector<std::string>	tokens;

	command[command.size()] = '\0';
	while (std::getline(ss, item, ' '))
		tokens.push_back(item);

	if (tokens.size() < 3) {
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NEEDMOREPARAMS + " * :Not enough parameters", client);
		return ;
	}

	if (tokens[1][0] != '#') {
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	tokens[1].erase(0, 1);

	Channel *channel = getChannel(tokens[1]);
	if (not channel) {
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOSUCHCHANNEL + " * :No such channel", client);
		return ;
	}

	if (not channel->isInChannel(client)) {
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_NOTONCHANNEL + " * :You're not on that channel", client);
		return ;
	}

	if (not channel->isOperator(client)) {
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_CHANOPRIVSNEEDED + " * :You're not an operator in that channel", client);
		return ;
	}

	Client *client_to_invite = getClientFromNick(tokens[2]);
	if (not client_to_invite) {
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver " + Errors::ERR_USERNOTINCHANNEL + " * :User doesn't exist", client);
		return ;
	}

	if (channel->isInChannel(client_to_invite)) {
		std::cout << RED << "Invalid command sent by " << client->getUsername() << " : " << YELLOW << command << ENDL;
		sendToClient(": serverserver   * :User is already in that channel", client);
		return ;
	}

	channel->addInvited(client_to_invite);
	sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " INVITE " + client_to_invite->getNickname() + " #" + channel->getName(), client_to_invite);
}
