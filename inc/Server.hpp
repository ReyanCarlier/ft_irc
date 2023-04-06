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
		int			socket_fd;
		int			opt;
		int			addrlen;
		int			accept_fd;
		int			port;
		char		*password;
		char		buffer[1024];
		sockaddr_in	address;
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
