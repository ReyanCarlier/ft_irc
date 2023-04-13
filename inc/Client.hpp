/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 18:00:01 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/13 13:47:53 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <header.hpp>

/**
 * Representation of a client.
 */
class Client
{
	public:
		/* Constructor & Destructor ***************************************** */
		Client();
		Client(int _fd);
		Client(Client const &src);
		~Client();

		/* Operator overload ************************************************ */
		Client&		operator=(Client const &rhs);

		/* Getter *********************************************************** */
		int			getSocket(void);
		std::string	getMessage(void);
		std::string getUsername(void);
		std::string getHostname(void);
		std::string getRealName(void);
		std::string getNickname(void);
		int			isReady(void);
		int			isWelcomed(void);
		int			getId(void);
		int			getMode(void);

		/* Setter *********************************************************** */
		void		setNickname(std::string nickname);
		void		setSocket(int socket);
		void		setMessage(std::string	messageentrant);
		void		setUsername(std::string name);
		void		setHostname(std::string name);
		void		setRealName(std::string name);
		void		setHost(std::string name);
		void		setOk(int ok);
		void		setWelcomed(int bvn);
		void		setId(int id);
		void		setMode(int mode);

		/* Function ********************************************************* */
	private:
		int			_id;
		int			_mode;
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _host;
		std::string _realname;
		int			_socket;
		std::string	_messageentrant;
		int			_isok;
		int			_bvn;
};
