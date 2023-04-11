/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:21:29 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 15:16:34 by recarlie         ###   ########.fr       */
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
		int					_master_socket_fd;
		int					_addrlen;
		int					_accept_fd;
		int					_opt;
		int					_port;
		char				*_password;
		char				_buffer[1024];
		sockaddr_in			_address;
		std::list<Message>	_messages;
		std::list<Channel>	_channels;
		std::list<Client>	_clients;
	public:
		/* Constructor & Destructor ***************************************** */
		Server(char **av);
		~Server();

		/* Getter *********************************************************** */
		int		getMasterSocket(void);
		int		getAccept(void);
		char	*getBuffer(void);

		/* Setter *********************************************************** */
		void	setAccept(void);
		void	setAddress(void);

		/* Function ********************************************************* */
		int			*getPtrOpt(void);
		sockaddr	*getCastAddress(void);
};

#endif
