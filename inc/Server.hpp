#pragma once

#include <header.hpp>
#include <ctime>
#include <vector>
#include <ostream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "Client.hpp"
#include "Channel.hpp"
#include "Errors.hpp"

class Server
{
	private:
		int						_addrlen;
		int						_accept_fd;
		int						_max_clients;
		int						_opt;
		std::string				_password;
		std::string				_passwordadmin;
		char					_buffer[BUFFER_SIZE];
		std::vector<Channel *>	_channels;
		std::vector<Client *>	_clients;
		sockaddr_in				_address;
		int						_port;
		int						_socket_fd;
		int						_clientinqueue;
		bool					_die;
	public:
		Server(char **av);
		~Server();

		int						getSocketFd(void);
		int						getAccept(void);
		int						getPort(void);
		char					*getBuffer(void);
		Client					*getClient(int fd);
    	Client					*getClientFromNick(std::string nick);
		int						getHighestFd(fd_set*, fd_set*);
		std::string				getPassword(void);
		bool					getDie(void);

		void					setAccept(void);
		void					setAddress(void);
		void					setPort(int port);
		void					setDie(Client *client);

		void					addClient(Client *client);
		void					addChannel(Channel *channel);
		void					removeClient(Client *client);
		void					removeChannel(Channel *channel);

		void					commandHandler(std::string command, Client *client);

		void					welcome(Client *client);

		std::vector<Client *>	getClients(void);
		std::vector<Channel *>	getChannels(void);
		Channel					*getChannel(std::string channel_name);
		std::string				getpassadmin(void);
		void					setpassadmin(std::string pass);

		int						*getPtrOpt(void);
		sockaddr				*getCastAddress(void);

		void					addClientQueue(void);
		void					removeClientQueue(void);
		int						getClientQueue(void);

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
		void					topic(std::string command, Client *client);
		void					part(std::string command, Client *client);
		void					quit(std::string command, Client *client);
		void					privmsg(std::string command, Client *client);
		void					kick(std::string command, Client *client);
		void					ban(std::string command, Client *client);
		void					unban(std::string command, Client *client);
		void					invite(std::string command, Client *client);
		void					oper(std::string command, Client *client);
		void					kill(std::string command, Client *client);
		void					time(std::string command, Client *client);
		void					mute(std::string command, Client *client);
		void					unmute(std::string command, Client *client);
		void					list(std::string command, Client *client);
};
