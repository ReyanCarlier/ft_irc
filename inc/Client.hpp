#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>

class Client
{

	public:

		Client();
		Client( Client const & src );
		~Client();

		Client &		operator=( Client const & rhs );

		void	SetNick(std::string nickname);
		void	SetSocket(int socket);
		int		GetSocket(void);
		void	SetMessage(std::string	messageentrant);
		std::string	GetMessage(void);
		void	SetUserName(std::string name);
		void	SetHostName(std::string name);
		void	SetRealName(std::string name);
		void	SetHost(std::string name);
		void	Setok(int ok);
		std::string getUserName(void);
		int		getok(void);
		int		getbvn(void);
		void	setbvn(int bvn);

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