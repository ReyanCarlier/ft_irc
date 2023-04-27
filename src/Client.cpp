/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 18:00:22 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/24 15:54:04 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

Client::Client() :
_socket(0), _isok(0), _bvn(1), _pass(2), _isadmin(0) {
	cout << "Client created" << endl;
}

Client::Client(int _fd) :
_socket(_fd), _isok(0), _bvn(1), _pass(2), _isadmin(0) {
	cout << "Client created" << endl;
}

Client::Client(const Client &src) {
	*this = src;
}

Client::~Client() {
	close(_socket);
}

Client&		Client::operator=(Client const &rhs) {
	_nickname = rhs._nickname;
	_username = rhs._username;
	_hostname = rhs._hostname;
	_host = rhs._host;
	_realname = rhs._realname;
	return (*this);
}

int			Client::getSocket(void) {
	return (_socket);
}

string	Client::getMessage(void) {
	return (_messageentrant);
}

string Client::getUsername(void) {
	return (_username);
}

int			Client::isReady(void) {
	return (_isok);
}

int			Client::isWelcomed(void) {
	return (_bvn);
}

int			Client::getId(void) {
	return (_id);
}

int			Client::getPass(void) {
	return (_pass);
}

string	Client::getHostname(void) {
	return (_hostname);
}

string	Client::getRealName(void) {
	return (_realname);
}

string	Client::getNickname(void) {
	return (_nickname);
}

string	Client::getHost(void) {
	return (_host);
}

void		Client::setId(int id) {
	_id = id;
}

void		Client::setSocket(int socket) {
	_socket = socket;
}

void		Client::setMessage(string	messageentrant) {
	_messageentrant = messageentrant;
}

void		Client::setNickname(string nickname) {
	_nickname = nickname;
}

void		Client::setUsername(string name) {
	_username = name;
}

void		Client::setHostname(string name) {
	_hostname = name;
}

void		Client::setHost(string name) {
	_host = name;
}

void		Client::setMode(int mode) {
	_mode = mode;
}

void		Client::setRealName(string name) {
	_realname = name;
}

void		Client::setOk(int ok) {
	_isok = ok;
}

void		Client::setWelcomed(int bvn) {
	_bvn = bvn;
}

void		Client::setPass(int status) {
	_pass = status;
}

void		Client::setAdmin(bool status) {
	_isadmin = status;
}

bool		Client::isAdmin(void) {
	return (_isadmin);
}

void		Client::setBuffer(string buffer) {
	_buffer = buffer;
}

string	Client::getBuffer(void) {
	return (_buffer);
}
