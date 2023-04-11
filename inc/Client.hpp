/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 18:00:01 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 17:05:39 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <header.hpp>

/**
 * Representation of a client.
 * 
 */
class Client
{
	public:
		/* Constructor & Destructor ***************************************** */
		Client();
		Client(Client const &src);
		~Client();

		/* Operator overload ************************************************ */
		Client&		operator=(Client const &rhs);

		/* Getter *********************************************************** */
		int			getSocket(void);
		std::string	GetMessage(void);
		std::string getUserName(void);
		int			getok(void);
		int			getbvn(void);
		int			getId();

		/* Setter *********************************************************** */
		void		SetNick(std::string nickname);
		void		setSocket(int socket);
		void		SetMessage(std::string	messageentrant);
		void		SetUserName(std::string name);
		void		SetHostName(std::string name);
		void		SetRealName(std::string name);
		void		SetHost(std::string name);
		void		Setok(int ok);
		void		setbvn(int bvn);
		void		setId(int id);

		/* Function ********************************************************* */
	private:
		int			_id;
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