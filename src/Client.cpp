#include <Client.hpp>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client(): _socket(0)
{
	_isok = 0;
	_bvn = 1;
}

Client::Client( const Client & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &				Client::operator=( Client const & rhs )
{
	_nickname = rhs._nickname;
	return *this;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void	Client::SetSocket(int socket)
{
	_socket = socket;
}

int		Client::GetSocket(void)
{
	return(_socket);
}

void	Client::SetMessage(std::string	messageentrant)
{
	_messageentrant = messageentrant;
}

std::string	Client::GetMessage(void)
{
	return(_messageentrant);
}

void	Client::SetNick(std::string nickname)
{
	_nickname = nickname;
}

void	Client::SetUserName(std::string name)
{
	_username = name;
}

void	Client::SetHostName(std::string name)
{
	_hostname = name;
}

void	Client::SetHost(std::string name)
{
	_host = name;
}

void	Client::SetRealName(std::string name)
{
	_realname = name;
}

void	Client::Setok(int ok)
{
	_isok = ok;
}


std::string Client::getUserName(void)
{
	return(_username);
}

int	Client::getok(void)
{
	return (_isok);
}

void	Client::setbvn(int bvn)
{
	_bvn = bvn;
}

int Client::getbvn(void)
{
	return(_bvn);
}

/* ************************************************************************** */