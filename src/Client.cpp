/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 18:00:22 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 17:44:44 by nfelsemb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

/* Constructor ************************************************************** */

Client::Client() : _socket(0), _isok(0), _bvn(1)
{
	std::cout << "Client created." << std::endl;
}

Client::Client(const Client &src)
{
	*this = src;
}

/* Destructor *************************************************************** */

Client::~Client()
{
	std::cout << "Client " << _nickname << " deleted." << std::endl;
}

/* Operator overload ******************************************************** */
Client&		Client::operator=(Client const &rhs)
{
	_nickname = rhs._nickname;
	_username = rhs._username;
	_hostname = rhs._hostname;
	_host = rhs._host;
	_realname = rhs._realname;
	return (*this);
}

/* Getter ******************************************************************* */

int			Client::getSocket(void)
{
	return (_socket);
}

std::string	Client::GetMessage(void)
{
	return (_messageentrant);
}

std::string Client::getUserName(void)
{
	return (_username);
}

int			Client::getok(void)
{
	return (_isok);
}

int			Client::getbvn(void)
{
	return (_bvn);
}

int			Client::getId(void)
{
	return (_id);
}

/* Setter ******************************************************************* */

void		Client::setId(int id)
{
	_id = id;
}

void		Client::setSocket(int socket)
{
	_socket = socket;
}

void		Client::SetMessage(std::string	messageentrant)
{
	_messageentrant = messageentrant;
}

void		Client::SetNick(std::string nickname)
{
	_nickname = nickname;
}

void		Client::SetUserName(std::string name)
{
	std::cout << "username of " << _username << "set to " << name << std::endl;
	_username = name;
}

void		Client::SetHostName(std::string name)
{
	_hostname = name;
}

void		Client::SetHost(std::string name)
{
	_host = name;
}

void		Client::SetRealName(std::string name)
{
	_realname = name;
}

void		Client::Setok(int ok)
{
	_isok = ok;
}

void		Client::setbvn(int bvn)
{
	_bvn = bvn;
}

/* ************************************************************************** */
