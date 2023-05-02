/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommand.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 14:43:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/27 14:43:33 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

void				Server::nick(string command, Client *client) {
	stringstream 	ss(command);
	string			item;
	vector<string>	tokens;
	string			old_nickname;

	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 2) {
		cout << RED << "Invalid command sent by " << client->getNickname()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NONICKNAMEGIVEN +
		" * :No nickname given", client);
		return ;
	}
	if (tokens.size() > 2) {
		cout << RED << "Invalid command sent by " << client->getNickname()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_ERRONEUSNICKNAME +
		" * :Erroneous nickname", client);
		return ;
	}
	old_nickname = client->getNickname();
	if (this->getClientFromNick(tokens[1]) != NULL) {
		sendToClient(":serverserver " + Errors::ERR_NICKNAMEINUSE + " * " +
		tokens[1] + " :Nickname is already in use.", client);
		return ;
	}
	client->setNickname(tokens[1]);
	if (client->isWelcomed() == 0) {
		sendToClient(":" + old_nickname + " NICK :" + client->getNickname(),
		client);
	}
	vector<Channel*>	channels;
	for (CH_IT = _channels.begin(); it != _channels.end(); it++) {
		if ((*it)->isInChannel(client))
			channels.push_back((*it));
	}
	if (channels.size() > 0) {
		for (CH_IT = channels.begin(); it != channels.end(); it++) {
			vector<Client*> clients = (*it)->getClients();
			for (CL_IT2 = clients.begin(); it2 != clients.end(); it2++) {
				if ((*it2) != client) {
					sendToClient(":" + old_nickname + " NICK :" +
					client->getNickname(), (*it2));
				}
			}
		}
	}
}

void				Server::user(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 4) {
		cout << RED << "Invalid command sent by " << client->getSocket()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (client->getPass() == 0) {
		removeClient(client);
		return ;
	}
	client->setUsername(tokens[1]);
	client->setHost(tokens[2]);
	client->setHostname(tokens[3]);
	string realname;
	if (tokens.size() >= 5) {
		if (tokens[4][0] == ':')
			tokens[4].erase(0, 1);
		for (size_t i = 4; i < tokens.size(); i++) {
			realname.append(tokens[i]);
			if (i != tokens.size() - 1)
				realname.append(" ");
		}
	} else {
		realname = client->getUsername() + "!" + client->getHost() + "@" +
		client->getHostname();
	}
	client->setRealName(realname);
	client->setOk(1);
}

void				Server::ping(Client *client) {
	sendToClient(":serverserver PONG serverserver :" + client->getUsername() +
	"\r\n", client);
}

void				Server::pass(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 2) {
		cout << RED << "Invalid command sent by client " << client->getSocket()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		removeClient(client);
		return ;
	}
	if (tokens.size() > 2) {
		cout << RED << "Invalid command sent by client " << client->getSocket()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Too many parameters", client);
		removeClient(client);
		return ;
	}
	if (tokens[1] != this->getPassword()) {
		cout << RED << "Invalid password sent by client " << client->getSocket()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_PASSWDMISMATCH +
		" * :Password incorrect", client);
		removeClient(client);
	} else {
		client->setPass(1);
	}
}

void				Server::mode(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() >= 2 && startwith("#", tokens[1])) {
		tokens[1].erase(0, 1);
		Channel*	channel = getChannel(tokens[1]);
		if (channel == NULL) {
			cout << RED << "Invalid command sent by " << client->getUsername()
			<< " : " << YELLOW << command << ENDL;
			sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
			" * :No such channel", client);
			return ;
		}
		if (tokens.size() == 2) {
			string	buffer = " +";
			if (channel->isInviteOnly())
				buffer += "i";
			if (channel->getTopic() != "No topic yet")
				buffer += "n";
			if (buffer.size() == 2)
				buffer = "";
			sendToClient(":serverserver 324 " + client->getNickname() + " #" +
			channel->getName() + " " + buffer, client);
			return ;
		} else {
			if (channel->isInChannel(client) == false) {
				cout << RED << "Invalid command sent by "
				<< client->getUsername() << " : " << YELLOW << command << ENDL;
				sendToClient(":serverserver " + Errors::ERR_NOTONCHANNEL +
				" * :You're !on that channel", client);
				return ;
			}
			if (channel->isOperator(client) == false) {
				cout << RED << "Invalid command sent by "
				<< client->getUsername() << " : " << YELLOW << command << ENDL;
				sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED +
				" * :You're !channel operator", client);
				return ;
			}
			if (tokens[2] == "+b") {
				if (tokens.size() < 4) {
					cout << RED << "Invalid command sent by " <<
					client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
					" * :Not enough parameters", client);
					return ;
				}
				Client*	target = NULL;
				string	username = tokens[3];
				username.erase(0, 3);
				username = username.substr(0, username.find('@'));
				for (CL_IT = _clients.begin(); it != _clients.end(); it++) {
					if ((*it)->getUsername() == username &&
					(*it)->getNickname() != client->getNickname()) {
						target = (*it);
						break ;
					}
				}
				if (target == NULL) {
					cout << RED << "Invalid command sent by " <<
					client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(":serverserver " + Errors::ERR_NOSUCHNICK +
					" * :No such nick", client);
					return ;
				}
				string	reason = "";
				if (tokens.size() > 4) {
					for (size_t i = 4; i < tokens.size(); i++)
						reason += tokens[i] + " ";
					reason = reason.substr(0, reason.size() - 1);
				} else {
					reason = "Banned by " + client->getUsername();
				}
				vector<Client*>	clients = channel->getClients();
				for (CL_IT = clients.begin(); it != clients.end(); it++) {
					sendToClient(":serverserver MODE #" + channel->getName() +
					" +b !" + target->getNickname() + "@*", (*it));
					if ((*it)->getNickname() != target->getNickname()) {
						sendToClient(":serverserver KICK #" + channel->getName()
						+ " " + target->getNickname() + " :" + reason, (*it));
					}
				}
				part("PART #" + channel->getName() + " :" + reason, target);
				channel->addBanned(target);
				who("WHO #" + channel->getName(), client);
				return ;
			} else if (tokens[2] == "-b") {
				if (tokens.size() < 4) {
					cout << RED << "Invalid command sent by " <<
					client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
					" * :Not enough parameters", client);
					return ;
				}
				Client*			target = NULL;
				vector<Client*>	banned = channel->getBanned();
				string			nickname = tokens[3];
				nickname.erase(0, 1);
				nickname = nickname.substr(0, nickname.find('@'));
				for (CL_IT = banned.begin(); it != banned.end(); it++) {
					if ((*it)->getNickname() == nickname) {
						target = (*it);
						break ;
					}
				}
				if (target == NULL) {
					cout << RED << "Invalid command sent by " <<
					client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(
						":serverserver " + Errors::ERR_NOSUCHNICK +
						" * :Target isn't in blacklist of #" +
						channel->getName(), client
					);
					return ;
				}
				channel->unbanClient(target);
				vector<Client*>	clients = channel->getClients();
				for (CL_IT = clients.begin(); it != clients.end(); it++)
					sendToClient(":serverserver MODE #" + channel->getName() +
					" -b !" + target->getNickname() + "@*", (*it));
				return ;
			} else if (tokens[2] == "+i") {
				if (channel->isInviteOnly() == true) {
					cout << RED << "Invalid command sent by " <<
					client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED
					+ " * :Channel is already invite only", client);
					return ;
				}
				channel->setInviteOnly(true);
				vector<Client*>	clients = channel->getClients();
				for (CL_IT = clients.begin(); it != clients.end(); it++) {
					sendToClient(":serverserver MODE #" + channel->getName() +
					" +i", (*it));
				}
				return ;
			} else if (tokens[2] == "-i") {
				if (channel->isInviteOnly() == false) {
					cout << RED << "Invalid command sent by " <<
					client->getUsername() << " : " << YELLOW << command << ENDL;
					sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED
					+ " * :Channel is !invite only", client);
					return ;
				}
				channel->setInviteOnly(false);
				vector<Client*>	clients = channel->getClients();
				for (CL_IT = clients.begin(); it != clients.end(); it++) {
					sendToClient(":serverserver MODE #" + channel->getName() +
					" -i", (*it));
				}
			}
		}
	}
	if (tokens.size() < 3) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (tokens[2] == "+i") {
		client->setMode(1);
	} else if (tokens[2] == "-i") {
		client->setMode(0);
	} else {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_UMODEUNKNOWNFLAG +
		" * :Unknown MODE flag", client);
		return ;
	}
	sendToClient(":serverserver MODE " + client->getUsername() + " " +
	tokens[2], client);
}

void				Server::privmsg(string command, Client *client) {
	vector<string>	tokens;
	stringstream	ss(command);
	string			item;
	string			message;

	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 3) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (tokens[1][0] == '#') {
		Channel*	channel = getChannel(
		tokens[1].substr(1, tokens[1].size() - 1));

		if (!channel) {
			sendToClient(":serverserver 403 " + client->getUsername() + " " +
			tokens[1] + " :No such channel\r\n", client);
			return ;
		}
		message = tokens[2];
		if (message[0] == ':')
			message.erase(0, 1);
		for (size_t i = 3; i < tokens.size(); i++) {
			message.append(" ");
			message.append(tokens[i]);
		}
		if (!channel->isInChannel(client)) {
			cout << RED << "Invalid command sent by " << client->getUsername()
			<< " : " << YELLOW << command << ENDL;
			sendToClient(":serverserver " + Errors::ERR_NOTONCHANNEL +
			" * :You're !on that channel", client);
			return ;
		}
		if (channel->isMuted(client)) {
			cout << RED << "Invalid command sent by " << client->getUsername()
			<< " : " << YELLOW << command << ENDL;
			sendToClient(":serverserver " + Errors::ERR_CANNOTSENDTOCHAN +
			" * :Cannot speak in this channel, you're muted.", client);
			return ;
		}
		for (size_t i = 0; i < channel->getClients().size(); i++) {
			if (channel->getClients()[i] != client) {
				sendToClient(
					":" + client->getNickname() + "!" + client->getUsername() +
					"@" + client->getHostname() + " PRIVMSG " + tokens[1] + " :"
					+ message,
					channel->getClients()[i]
				);
			}
		}
	} else {
		Client*	target;

		target = getClientFromNick(tokens[1]);
		if (!target) {
			sendToClient(":serverserver 401 " + client->getUsername() + " " +
			tokens[1] + " :No user with this nickname.", client);
			return ;
		}
		message = tokens[2];
		for (size_t i = 3; i < tokens.size(); i++) {
			message.append(" ");
			message.append(tokens[i]);
		}
		sendToClient(":" + client->getNickname() + " PRIVMSG " +
		target->getNickname() + " " + message, target);	
	}
}

void				Server::join(string command, Client *client) {
	string	channel_name;

	command.erase(0, 5);
	bool	added = false;
	if (command[0] == '#') {
		channel_name = command;
		channel_name.erase(0, 1);
	} else {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	Channel*	channel_to_join = getChannel(channel_name);
	if (!channel_to_join) {
		channel_to_join = new Channel(channel_name);
		addChannel(channel_to_join);
		channel_to_join->addClient(client);
		added = true;
		channel_to_join->addOperator(client);
	}
	if (client->isAdmin() && !added) {
		channel_to_join->addOperator(client);
	} else {
		if (channel_to_join->isBanned(client)) {
			cout << RED << "Client " << client->getUsername() << " cannot join "
			<< channel_name << " because banned." << ENDL;
			sendToClient(":serverserver " + Errors::ERR_BANNEDFROMCHAN +
				" * :Cannot join channel " + channel_name +
				" because you're banned.", client
			);
			return ;
		}
		if (channel_to_join->isInviteOnly() &&
			!channel_to_join->isInvited(client)
		) {
			cout << RED << "Client " << client->getUsername() << " cannot join "
			<< channel_name << " because invite only." << ENDL;
			sendToClient(":serverserver " + Errors::ERR_INVITEONLYCHAN +
			" * :Cannot join channel (+i)", client);
			return ;
		}
	}
	if (!added)
		getChannel(channel_name)->addClient(client);
	sendToClient(":serverserver 332 " + client->getNickname() + " " +
	channel_name + " :" + channel_to_join->getTopic(), client);
	
	string	buffer;
	for (size_t i = 0; i < channel_to_join->getClients().size(); i++) {
		if (channel_to_join->isOperator(channel_to_join->getClients().at(i)) ||
		channel_to_join->getClients().at(i)->isAdmin())
			buffer += "@";
		buffer += channel_to_join->getClients()[i]->getNickname();
		if (i != channel_to_join->getClients().size() - 1)
			buffer += " ";
	}
	sendToClient(":serverserver 353 " + client->getNickname() + " = " +
	channel_name + " :" + buffer, client);
	sendToClient(":serverserver 366 " + client->getNickname() + " " +
	channel_name + " :End of /NAMES list.", client);
	for (size_t i = 0; i < channel_to_join->getClients().size(); i++) {
		sendToClient(":" + client->getNickname() + "!" + client->getUsername() +
		"@" + client->getHostname() + " JOIN #" + channel_name,
		channel_to_join->getClients()[i]);
	}
}

void				Server::who(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() >= 2 && tokens[1].at(0) == '#') {
		tokens[1].erase(0, 1);
		Channel*	channel = getChannel(tokens[1]);
		if (channel == NULL) {
			cout << RED << "Invalid command sent by " << client->getUsername()
			<< " : " << YELLOW << command << ENDL;
			sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
			" * :No such channel", client);
			return ;
		}
		if (tokens.size() == 2) {
			vector<Client*>	clients = channel->getClients();
			for (CL_IT = clients.begin(); it != clients.end(); it++) {
				string	buffer(":serverserver 352 " + client->getNickname() +
				" " + channel->getName() + " " + (*it)->getUsername() + " " +
				(*it)->getHostname() + " " + (*it)->getHost() + " " +
				(*it)->getUsername() + " H :0 " + (*it)->getRealName());
				sendToClient(buffer, client);
			}
			sendToClient(":serverserver 315 " + client->getNickname() + " " +
			channel->getName() + " :End of /WHO list.", client);
		}
	}
}

void				Server::topic(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 2) {
		sendToClient(":serverserver 461 TOPIC :Not enough parameters", client);
		return ;
	}
	tokens[1][tokens[1].size()] = '\0';
	tokens[1].erase(0, 1);
	Channel*	channel = getChannel(tokens[1]);
	if (channel == NULL) {
		sendToClient(":serverserver 403 " + client->getUsername() + " " +
		tokens[1] + " :No such channel", client);
		return ;
	}
	if (channel->getOperators().size() > 0 && !channel->isOperator(client)) {
		sendToClient(":serverserver 482 " + client->getUsername() + " " +
		tokens[1] + " :You're !channel operator", client);
		return ;
	}
	if (tokens.size() == 2) {
		if (channel->getTopic() == "") {
			sendToClient(":serverserver 331 " + client->getUsername() + " #" +
			tokens[1] + " No topic is set", client);
		} else {
			sendToClient(":serverserver 332 " + client->getUsername() + " #" +
			tokens[1] + " " + channel->getTopic(), client);
		}
	} else {
		string	topic = "";
		for (size_t i = 2; i < tokens.size(); i++) {
			if (i == 2 && tokens[2][0] == ':')
				tokens[2].erase(0, 1);
			topic.append(tokens[i]);
			if (i != tokens.size() - 1)
				topic.append(" ");
		}
		channel->setTopic(topic);
		vector<Client*>	clients = channel->getClients();
		for (size_t i = 0; i < clients.size(); i++) {
			sendToClient(":" + client->getNickname() + "!" +
			client->getUsername() + "@" + client->getHostname() + " TOPIC #" +
			tokens[1] + " :" + topic, clients[i]);
			sendToClient(":serverserver 332 " + client->getUsername() + " #" +
			tokens[1] + " :" + topic, clients[i]);
		}
	}
}

void				Server::part(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;
	string			channel_name;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() == 1) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	channel_name = tokens[1].erase(0, 1);

	Channel*	channel = getChannel(channel_name);
	if (!channel) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	if (!channel->isInChannel(client)) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOTONCHANNEL +
		" * :You're !on that channel", client);
		return ;
	}
	for (size_t i = 0; i < channel->getClients().size(); i++) {
		if (channel->getClients()[i] == client) {
			if (channel->isOperator(client))
				channel->removeOperator(client);
			if (channel->isInvited(client))
				channel->removeInvited(client);
			for (size_t i = 0; i < channel->getClients().size(); i++) {
				sendToClient(":" + client->getNickname() + "!" +
				client->getUsername() + "@" + client->getHostname() + " PART #"
				+ channel_name + " :Leaving channel",channel->getClients()[i]);
			}
			if (channel->getClients().size() == 1) {
				removeChannel(channel);
			} else {
				channel->removeClient(client);
			}
		}
	}
}

void				Server::die(Client *client) {
	if (client->isAdmin()) {
		_die = true;
	} else {
		sendToClient(":serverserver 481 " + client->getNickname() +
		" :Permission Denied- You're !an IRC operator", client);
	}
}

void				Server::kick(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 3) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (tokens[1][0] != '#') {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	tokens[1].erase(0, 1);

	Channel*	channel = getChannel(tokens[1]);
	if (!channel) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	if (!channel->isInChannel(client)) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOTONCHANNEL +
		" * :You're !on that channel", client);
		return ;
	}
	if (!channel->isOperator(client)) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED +
		" * :You're !an operator in that channel", client);
		return ;
	}

	Client	*toKick = getClientFromNick(tokens[2]);
	if (!toKick) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_USERNOTINCHANNEL +
		" * :User doesn't exist", client);
		return ;
	}
	if (!channel->isInChannel(toKick)) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_USERNOTINCHANNEL +
		" * :User is !in that channel", client);
		return ;
	}
	if (channel->isOperator(toKick)) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED +
		" * :You can't kick an operator", client);
		return ;
	}
	string	reason = "No reason.";
	if (tokens.size() > 3) {
		tokens[3].erase(0, 1);
		if (tokens[3].size() > 0) {
			for (size_t i = 3; i < tokens.size(); i++)
				reason += tokens[i] + " ";
		}
	}
	if (reason == "No reason.")
		reason = "Kicked by " + client->getNickname() + ".";
	part("PART #" + channel->getName() + " :" + reason, toKick);
	for (size_t i = 0; i < channel->getClients().size(); i++) {
		sendToClient(":" + client->getNickname() + "!" + client->getUsername() +
		"@" + client->getHostname() + " KICK #" + channel->getName() + " " +
		toKick->getNickname() + " :" + reason, channel->getClients()[i]);
	}
	sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@"
	+ client->getHostname() + " KICK #" + channel->getName() + " " +
	toKick->getNickname() + " :" + reason, toKick);
}

void				Server::ban(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);

	if (tokens.size() < 3) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (tokens[1][0] != '#') {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	tokens[1].erase(0, 1);
	
	Channel*	channel = getChannel(tokens[1]);
	if (!channel) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	if (!channel->isInChannel(client)) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOTONCHANNEL +
		" * :You're !on that channel", client);
		return ;
	}
	if (!channel->isOperator(client)) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED +
		" * :You're !an operator in that channel", client);
		return ;
	}
	Client*	client_to_ban = getClientFromNick(tokens[2]);
	if (!client_to_ban) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_USERNOTINCHANNEL +
		" * :User doesn't exist", client);
		return ;
	}
	if (!channel->isInChannel(client_to_ban)) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_USERNOTINCHANNEL +
		" * :User is !in that channel", client);
		return ;
	}
	if (channel->isOperator(client_to_ban)) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED +
		" * :You can't ban an operator", client);
		return ;
	}
	string	reason = "No reason.";
	if (tokens.size() > 3) {
		tokens[3].erase(0, 1);
		if (tokens[3].size() > 0) {
			for (size_t i = 3; i < tokens.size(); i++)
				reason += tokens[i] + " ";
		}
	}
	if (reason == "No reason.")
		reason = "Banned by " + client->getNickname() + ".";
	
	vector<Client*>	clients = channel->getClients();

	for (size_t i = 0; i < clients.size(); i++)
		sendToClient(":serverserver MODE #" + channel->getName() + " +b !" +
		client_to_ban->getNickname() + "@*", clients[i]);
	kick("KICK #" + channel->getName() + " " + client_to_ban->getNickname() +
	" :" + reason, client);
	channel->addBanned(client_to_ban);
}

void				Server::unban(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 3) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (tokens[1][0] != '#') {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	tokens[1].erase(0, 1);

	Channel*	channel = getChannel(tokens[1]);
	if (!channel){
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	if (!channel->isInChannel(client)){
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOTONCHANNEL +
		" * :You're !on that channel", client);
		return ;
	}
	if (!channel->isOperator(client)){
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED +
		" * :You're !an operator in that channel", client);
		return ;
	}

	Client*	client_to_unban = getClientFromNick(tokens[2]);
	if (!client_to_unban) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_USERNOTINCHANNEL +
		" * :User doesn't exist", client);
		return ;
	}
	if (!channel->isInChannel(client_to_unban)) {
		cout << RED << "Invalid command sent by " << client->getUsername()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_USERNOTINCHANNEL +
		" * :User is !in that channel", client);
		return ;
	}
	channel->unbanClient(client_to_unban);
	for (size_t i = 0; i < channel->getClients().size(); i++) {
		sendToClient(":" + client->getNickname() + "!" + client->getUsername() +
		"@" + client->getHostname() + " MODE #" + channel->getName() + " -b " +
		client_to_unban->getNickname(), channel->getClients()[i]);
	}
}

void				Server::list(string command, Client *client) {
	string	str;

	if (command.find('#') == string::npos) {
		for (CH_IT = _channels.begin(); it != _channels.end(); it++) {
			stringstream	ss;
			ss << '#' << (*it)->getName() << ' '
			<< (*it)->getClients().size() << ' ' << (*it)->getTopic();
			str = ss.str();
			sendToClient(str, client);
		}
	} else {
		string				channel = command.substr(command.find('#') + 1);
		string::size_type	pos;
		string				substr;

		while (channel.find('#') != string::npos) {
			pos = channel.find(',');
			if (pos != string::npos)
				substr = channel.substr(0, pos);
			else
				substr = channel;
			for (CH_IT = _channels.begin(); it != _channels.end(); it++) {
				if (substr != (*it)->getName())
					continue ;
				stringstream	ss;
				ss << '#' << (*it)->getName() << ' '
				<< (*it)->getClients().size() << ' ' << (*it)->getTopic();
				str = ss.str();
				sendToClient(str, client);
			}
			channel.erase(0, substr.size() + 1);
			channel = channel.substr(channel.find('#') + 1);
		}
		pos = channel.find(',');
		if (pos != string::npos)
			substr = channel.substr(0, pos);
		else
			substr = channel;
		for (CH_IT = _channels.begin(); it != _channels.end(); it++) {
			if (substr != (*it)->getName())
				continue ;
			stringstream	ss;
			ss << '#' << (*it)->getName() << ' '
			<< (*it)->getClients().size() << ' ' << (*it)->getTopic();
			str = ss.str();
			sendToClient(str, client);
		}
	}
}

void				Server::quit(string command, Client *client) {
	string	reason;

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
	cerr << RED << "Client " << client->getSocket()
	<< " disconnected. Reason : " << reason << ENDL;
	close(client->getSocket());
	removeClient(client);
}

void				Server::invite(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 3) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (tokens[1][0] != '#') {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	tokens[1].erase(0, 1);
	Channel*	channel = getChannel(tokens[1]);
	if (!channel) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL +
		" * :No such channel", client);
		return ;
	}
	if (!channel->isInChannel(client)) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NOTONCHANNEL +
		" * :You're !on that channel", client);
		return ;
	}
	if (!channel->isOperator(client)) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_CHANOPRIVSNEEDED +
		" * :You're !an operator in that channel", client);
		return ;
	}
	Client*	client_to_invite = getClientFromNick(tokens[2]);
	if (!client_to_invite) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_USERNOTINCHANNEL +
		" * :User doesn't exist", client);
		return ;
	}
	if (channel->isInChannel(client_to_invite)) {
		cout << RED << "Invalid command sent by " << client->getUsername() <<
		" : " << YELLOW << command << ENDL;
		sendToClient(":serverserver ERROR * :User is already in that channel", 
		client);
		return ;
	}
	channel->addInvited(client_to_invite);
	sendToClient(":" + client->getNickname() + "!" + client->getUsername() + "@"
	+ client->getHostname() + " INVITE " + client_to_invite->getNickname() +
	" #" + channel->getName(), client_to_invite);
}

void				Server::oper(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 3) {
		cout << RED << "Invalid command sent by " << client->getSocket()
		<< " : " << YELLOW << command << ENDL;
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (tokens[1] == client->getNickname()) {
		if (tokens[2] == this->getpassadmin()) {
			client->setAdmin(true);
			vector<Channel*>	listchan = this->getChannels();
			for (size_t i = 0; i < listchan.size(); i++) {
				Channel*	tmp = listchan.at(i);
				if (tmp->isInChannel(client)) {
					tmp->addOperator(client);
					vector<Client*>	channel_clients = tmp->getClients();
					for (CL_IT = channel_clients.begin();
					it != channel_clients.end(); it++) {
						sendToClient(":serverserver MODE #" + tmp->getName() +
						" +o " + client->getNickname(), (*it));
					}
				}
			}
			sendToClient(":serverserver 381 " + client->getNickname() +
			" :You are now an IRC operator", client);
			sendToClient(":serverserver MODE " + client->getNickname() +
			" +o", client);
		} else {
			sendToClient(":serverserver 464 " + client->getNickname() +
			" :Password incorrect", client);
		}
	} else {
		if (tokens[2] == this->getpassadmin()) {
			Client*	target;
			if ((target = this->getClientFromNick(tokens[1])) != NULL) {
				target->setAdmin(true);
				vector<Channel*>	listchan = this->getChannels();
				for (size_t i = 0; i < listchan.size(); i++) {
					Channel*	tmp = listchan.at(i);
					if (tmp->isInChannel(target)) {
						tmp->addOperator(target);
						vector<Client*>	channel_clients = tmp->getClients();
						for (CL_IT = channel_clients.begin();
						it != channel_clients.end(); it++) {
							sendToClient(":serverserver MODE #" + tmp->getName()
							+ " +o " + target->getNickname(), (*it));
						}
					}
				}
				sendToClient(":serverserver 381 " + target->getNickname() +
				" :You are now an IRC operator", target);
				sendToClient(":serverserver MODE " + target->getNickname() +
				" +o", target);
			} else {
				sendToClient(":serverserver 491 " + tokens[2] +
				" :No O-lines for your host", client);
			}
		} else {
			sendToClient(":serverserver 464 " + client->getNickname() +
			" :Password incorrect", client);
		}
	}
}

void				Server::kill(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() < 3) {
		sendToClient(":serverserver " + Errors::ERR_NEEDMOREPARAMS +
		" * :Not enough parameters", client);
		return ;
	}
	if (!client->isAdmin()) {
		sendToClient(":serverserver " + Errors::ERR_NOPRIVILEGES +
		" * :Permission Denied- You're !an IRC operator", client);
		return ;
	}
	Client*	client_to_kill = getClientFromNick(tokens[1]);
	if (client_to_kill == NULL) {
		sendToClient(":serverserver " + Errors::ERR_NOSUCHNICK +
		" * :No such nickname.", client);
		return ;
	}
	if (client_to_kill->isAdmin()) {
		sendToClient(":serverserver " + Errors::ERR_NOPRIVILEGES +
		" * :Permission Denied- You can't kill an IRC operator", client);
		return ;
	}
	string	reason = "";
	if (tokens.size() == 3) {
		if (tokens[2].size() == 1 && tokens[2][0] == ':')
			reason = "No reason.";
	} else {
		tokens[2].erase(0, 1);
	}
	if (reason != "No reason.") {
		for (size_t i = 2; i < tokens.size(); i++) {
			reason += tokens[i];
			if (i != tokens.size() - 1)
				reason += " ";
		}
	}
	sendToClient(":serverserver KILL " + client_to_kill->getNickname() + " :" +
	reason, client_to_kill);
	sendToClient(":serverserver KILL " + client_to_kill->getNickname() + " :" +
	reason, client);
	for (size_t i = 0; i < _channels.size(); i++) {
		Channel*	channel = _channels.at(i);
		if (channel->isBanned(client_to_kill))
			channel->unbanClient(client_to_kill);
		if (channel->isMuted(client_to_kill))
			channel->unmuteClient(client_to_kill);
		if (channel->isInvited(client_to_kill))
			channel->removeInvited(client_to_kill);
		if (channel->isInChannel(client_to_kill)) {
			part("PART #" + channel->getName() + " :KILLED BY AN IRC OPERATOR ("
			+ reason + ")", client_to_kill);
		}
	}
	removeClient(client_to_kill);
}

void				Server::time(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() != 1) {
		sendToClient(":serverserver ERROR * :Too much parameters", client);
		return ;
	}
	time_t	now = ::time(0);
	char*	date_time = ctime(&now);
	string	date = "";
	string	time = "";
	for (size_t i = 0; i < strlen(date_time); i++) {
		if (i < 10)
			date += date_time[i];
		if (i > 10 && i < 19)
			time += date_time[i];
	}
	sendToClient(":serverserver 391 " + client->getNickname() + " " + date + " "
	+ time + " :Server time", client);
}

void				Server::version(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() != 1) {
		sendToClient(":serverserver ERROR * :Too much parameters", client);
		return ;
	}
	sendToClient(":serverserver 351 " + client->getNickname() +
	" This awesome IRC Server was created by nfelsemb, frrusso and recarlie" +
	" and is currently in it's V1 !", client);
}

void				Server::names(string command, Client *client) {
	stringstream	ss(command);
	string			item;
	vector<string>	tokens;

	command[command.size()] = '\0';
	while (getline(ss, item, ' '))
		tokens.push_back(item);
	if (tokens.size() > 2) {
		sendToClient(":serverserver ERROR * :Too much parameters", client);
		return ;
	}
	if (tokens.size() < 2)
		sendToClient(":serverserver ERROR * :Not enough parameters", client);
	if (tokens[1][0] == '#') {
		tokens[1].erase(0, 1);
	} else {
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL + " * " +
		tokens[1] + " :No such channel", client);
		return ;
	}
	Channel*	channel = getChannel(tokens[1]);
	if (channel == NULL) {
		sendToClient(":serverserver " + Errors::ERR_NOSUCHCHANNEL + " * " +
		tokens[1] + " :No such channel", client);
		return ;
	}
	string	names = "";
	for (size_t i = 0; i < channel->getClients().size(); i++) {
		Client*	client = channel->getClients().at(i);
		if (client->getNickname() != "") {
			if (channel->isOperator(client))
				names += "@";
			if (client->getNickname() == channel->getClients().at(
			channel->getClients().size() - 1)->getNickname()) {
				names += client->getNickname();
			} else {
				names += client->getNickname() + " ";
			}
		}
	}
	sendToClient(":serverserver 353 " + client->getNickname() + " = " +
	channel->getName() + " :" + names, client);
}
