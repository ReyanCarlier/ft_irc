/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:21:29 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/12 14:37:47 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <header.hpp>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"

class Server
{
	private:
		int						_addrlen;
		int						_accept_fd;
		int						_max_clients;
		int						_opt;
		char					*_password;
		char					_buffer[1024];
		std::vector<Channel>	_channels;
		std::vector<Client *>		_clients;
		sockaddr_in				_address;
		int						_port;
		int						_socket_fd;
		// std::vector<Message>	_messages;
	public:
		/* Constructor & Destructor ***************************************** */
		Server();
		~Server();

		/* Getter *********************************************************** */
		int		getSocketFd(void);
		int		getAccept(void);
		int		getPort(void);
		char	*getBuffer(void);
		Client	*getClient(int fd);

		/* Setter *********************************************************** */
		void	setAccept(void);
		void	setAddress(void);
		void	setPort(int port);

		void	addClient(Client *client);
		void	addChannel(Channel channel);
		void	removeClient(Client *client);
		void	removeChannel(Channel channel);

		std::vector<Client *>		getClients(void);
		std::vector<Channel>	getChannels(void);

		/* Function ********************************************************* */
		int			*getPtrOpt(void);
		sockaddr	*getCastAddress(void);

		void		bind(int port);
		void		listen(void);
		void		run(void);
};
