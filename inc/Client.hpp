/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 18:00:01 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/07 18:00:08 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <header.hpp>

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
		int			GetSocket(void);
		std::string	GetMessage(void);
		std::string getUserName(void);
		int			getok(void);
		int			getbvn(void);

		/* Setter *********************************************************** */
		void		SetNick(std::string nickname);
		void		SetSocket(int socket);
		void		SetMessage(std::string	messageentrant);
		void		SetUserName(std::string name);
		void		SetHostName(std::string name);
		void		SetRealName(std::string name);
		void		SetHost(std::string name);
		void		Setok(int ok);
		void		setbvn(int bvn);

		/* Function ********************************************************* */
	private:
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

#endif /* ********************************************************** CLIENT_H */
