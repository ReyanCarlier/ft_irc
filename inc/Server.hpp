/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 13:21:29 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/07 15:08:04 by nfelsemb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <header.hpp>

class Server
{
	private:
		int			_master_socket_fd;
		int			_addrlen;
		int			_accept_fd;
		int			_max_client;
		int			_opt;
		int			_port;
		char		*_password;
		char		_buffer[1024];
		sockaddr_in	_address;
	public:
		/* Constructor & Destructor ***************************************** */
		Server(char **av);
		~Server();

		/* Getter *********************************************************** */
		int		getMasterSocket(void);
		int		getAccept(void);
		char	*getBuffer(void);
		int		getMaxClient(void);

		/* Setter *********************************************************** */
		void	setAccept(void);
		void	setAddress(void);

		/* Function ********************************************************* */
		int			*getPtrOpt(void);
		sockaddr	*getCastAddress(void);
};

#endif
