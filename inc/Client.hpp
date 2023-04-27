/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 18:00:01 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/17 15:09:46 by nfelsemb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <header.hpp>

class Client
{
	private:
		int			_id;
		int			_mode;
		string	_nickname;
		string	_username;
		string	_hostname;
		string	_host;
		string	_realname;
		string	_buffer;
		int			_socket;
		string	_messageentrant;
		int			_isok;
		int			_bvn;
		int			_pass;
		bool		_isadmin;
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
		string	getMessage(void);
		string getUsername(void);
		string getHostname(void);
		string getRealName(void);
		string getNickname(void);
		string getHost(void);
		int			isReady(void);
		int			isWelcomed(void);
		int			getPass();
		int			getId(void);
		int			getMode(void);
		string	getBuffer(void);

		/* Setter *********************************************************** */
		void		setNickname(string nickname);
		void		setSocket(int socket);
		void		setMessage(string	messageentrant);
		void		setUsername(string name);
		void		setHostname(string name);
		void		setRealName(string name);
		void		setHost(string name);
		void		setOk(int ok);
		void		setWelcomed(int bvn);
		void		setId(int id);
		void		setPass(int id);
		void		setMode(int mode);
		bool		isAdmin(void);
		void		setAdmin(bool status);
		void		setBuffer(string buffer);
};
