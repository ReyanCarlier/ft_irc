/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:21:29 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 16:57:02 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <header.hpp>
#include <list>
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"

class Server
{
	private:
		int					_addrlen;
		int					_accept_fd;
		int					_max_clients;
		int					_opt;
		
		char				*_password;
		char				_buffer[1024];
		
		
		std::list<Channel>	_channels;
		std::list<Client>	_clients;
		sockaddr_in			_address;
		int					_port;
		int					_socket_fd;

		//std::list<Message>	_messages;

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

		void	addClient(Client client);
		void	addChannel(Channel channel);
		void	removeClient(Client client);
		void	removeChannel(Channel channel);

		std::list<Client>	getClients(void);
		std::list<Channel>	getChannels(void);

		/* Function ********************************************************* */
		int			*getPtrOpt(void);
		sockaddr	*getCastAddress(void);

		void		bind(int port);
		void		listen(void);
		void		run(void);
};

#endif
