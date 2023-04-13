#pragma once

#include <header.hpp>
#include <vector>
#include <ostream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Errors.hpp"

class Server
{
	private:
		int						_addrlen;
		int						_accept_fd;
		int						_max_clients;
		int						_opt;
		std::string				_password;
		char					_buffer[1024];
		std::vector<Channel *>	_channels;
		std::vector<Client *>	_clients;
		sockaddr_in				_address;
		int						_port;
		int						_socket_fd;

	public:
		Server(char **av);
		~Server();

		int						getSocketFd(void);
		int						getAccept(void);
		int						getPort(void);
		char					*getBuffer(void);
		Client					*getClient(int fd);
		int						getHighestFd(fd_set*, fd_set*);
		std::string				getPassword(void);

		void					setAccept(void);
		void					setAddress(void);
		void					setPort(int port);

		void					addClient(Client *client);
		void					addChannel(Channel *channel);
		void					removeClient(Client *client);
		void					removeChannel(Channel *channel);

		void					commandHandler(std::string command, Client *client);

		// COMMANDS
		void					welcome(Client *client);

		std::vector<Client *>	getClients(void);
		std::vector<Channel *>	getChannels(void);
		Channel					*getChannel(std::string channel_name);

		int						*getPtrOpt(void);
		sockaddr				*getCastAddress(void);

		void					bind(int port);
		void					listen(void);
		void					run(void);

		bool					channelExists(std::string channel_name);
		void					sendToClient(std::string message, Client *client);
		void					nick(std::string command, Client *client);
		void					pass(std::string command, Client *client);
		void					user(std::string command, Client *client);
		void					ping(Client *client);
		void					mode(std::string command, Client *client);
		void					join(std::string command, Client *client);
		void					who(std::string command, Client *client);
};
