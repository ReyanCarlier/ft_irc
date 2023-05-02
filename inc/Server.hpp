/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:22:42 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/27 13:22:45 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "Errors.hpp"

class Server {
	private:
		int					_addrlen;
		int					_accept_fd;
		int					_max_clients;
		int					_opt;
		string				_password;
		string				_passwordadmin;
		char				_buffer[BUFFER_SIZE];
		vector<Channel*>	_channels;
		vector<Client*>		_clients;
		sockaddr_in			_address;
		int					_port;
		int					_socket_fd;
		int					_clientinqueue;
		bool				_die;

		/* Function ********************************************************* */
		bool				startwith(string prefix, string str);
	public:
		/* Constructor & Destructor ***************************************** */
		Server(char **av);
		~Server();

		/* Getter *********************************************************** */
		int					getSocketFd(void);
		int					getAccept(void);
		int					getPort(void);
		char*				getBuffer(void);
		Client*				getClient(int fd);
    	Client*				getClientFromNick(string nick);
		int					getHighestFd(fd_set*, fd_set*);
		string				getPassword(void);
		bool				getDie(void);

		/* Setter *********************************************************** */
		void				setAccept(void);
		void				setAddress(void);
		void				setPort(int port);

		/* Function ********************************************************* */
		void				addClient(Client *client);
		void				addChannel(Channel *channel);
		void				removeClient(Client *client);
		void				removeChannel(Channel *channel);
		void				commandHandler(string command, Client *client);
		void				welcome(Client *client);
		vector<Client*>		getClients(void);
		vector<Channel*>	getChannels(void);
		Channel*			getChannel(string channel_name);
		string				getpassadmin(void);
		void				setpassadmin(string pass);
		int*				getPtrOpt(void);
		sockaddr*			getCastAddress(void);
		void				addClientQueue(void);
		void				removeClientQueue(void);
		int					getClientQueue(void);
		void				bind(int port);
		void				listen(void);
		void				run(void);
		bool				channelExists(string channel_name);
		void				sendToClient(string message, Client *client);

		/* Command ********************************************************** */
		void				nick(string command, Client *client);
		void				user(string command, Client *client);
		void				ping(Client *client);
		void				pass(string command, Client *client);
		void				mode(string command, Client *client);
		void				privmsg(string command, Client *client);
		void				join(string command, Client *client);
		void				who(string command, Client *client);
		void				topic(string command, Client *client);
		void				part(string command, Client *client);
		void				die(Client *client);
		void				kick(string command, Client *client);
		void				ban(string command, Client *client);
		void				unban(string command, Client *client);
		void				list(string command, Client *client);
		void				quit(string command, Client *client);
		void				invite(string command, Client *client);
		void				oper(string command, Client *client);
		void				kill(string command, Client *client);
		void				time(string command, Client *client);
		void				version(string command, Client *client);
		void				names(string command, Client *client);
};
