/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:21:29 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/06 13:21:51 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <header.hpp>

class Server
{
	private:
		int			_socket_fd;
		int			_addrlen;
		int			_accept_fd;
		int			_port;
		char		*_password;
		char		_buffer[1024];
		sockaddr_in	_address;
	public:
		/* Constructor & Destructor ***************************************** */
		Server(char **av);
		~Server();

		/* Getter & Setter ************************************************** */
		int		getSocket(void);
		int		getAccept(void);
		char	*getBuffer(void);
		void	setAccept(void);
		void	setAddress(void);

		/* Function ********************************************************* */
		sockaddr	*getCastAddress(void);
};

#endif
