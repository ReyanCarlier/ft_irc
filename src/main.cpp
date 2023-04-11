/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:01:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 15:14:47 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <Client.hpp>

void	DebugMessage(std::string message)
{
	std::cout << YELLOW << message << std::endl;
}

bool	startwith(std::string prefix, std::string str)
{
	int	i = 0;

	if (prefix.length() > str.length())
		return (false);
	while (prefix[i])
	{
		if (prefix[i] != str[i])
			return (false);
		i++;
	}
	return (true);
}

bool	isport(char *av)
{
	if (strlen(av) > 5)
		return (false);
	for (size_t i = 0; i < strlen(av); i++)
	{
		if (isdigit(av[i]) == false)
			return (false);
	}
	if (atoi(av) > MAX_TCP_PORT)
		return (false);
	return (true);
}

int	main(int ac, char **av)
{
	/* Parsing ************************************************************** */
	if (ac != 3)
	{
		std::cout << YELLOW << "Usage: ./ircserv <port> <password>" << ENDL;
		return (0);
	}
	if (isport(av[1]) == false)
	{
		std::cerr << YELLOW << "Port \"" << av[1] << "\" is not good." << ENDL;
		return (0);
	}

	/* Variable ************************************************************* */
	Server	irc(av);
	Client	client[irc.getMaxClient()];

	fd_set		readfds, writefds;
	int			max_sd, sd, activity, valread;
	std::string	test, name;

	/* IRC server setup ***************************************************** */
	if (irc.getMasterSocket() == -1)
	{
		std::cerr << ERROR << "socket(): " << strerror(errno) << ENDL;
		return (1);
	}
	/* int setsockopt(int sockfd, int level, int optname,
	                  const void *optval, socklen_t optlen);
	irc.getMasterSocket(): manipulate options for the socket referred.
	SOL_SOCKET: To manipulate options at the sockets API level.
	SO_REUSEADDR: Reuse of local addresses is supported.
	SO_REUSEPORT: Allows multiple sockets to bind to the same address and port.
	irc.getPtrOpt(): Are used to access option values.
	sizeof(int): Type of optval.*/
	if (setsockopt(irc.getMasterSocket(), SOL_SOCKET,
	SO_REUSEADDR | SO_REUSEPORT, irc.getPtrOpt(), sizeof(int)))
	{
		std::cerr << ERROR << "setsockopt(): " << strerror(errno) << ENDL;
		return (1);
	}
	irc.setAddress();
	if (bind(irc.getMasterSocket(), irc.getCastAddress(), sizeof(sockaddr_in))
	== -1)
	{
		std::cerr << ERROR << "bind(): " << strerror(errno) << ENDL;
		return (1);
	}
	if (listen(irc.getMasterSocket(), MAX_REQUESTS) == -1)
	{
		std::cerr << ERROR << "listen(): " << strerror(errno) << ENDL;
		return (1);
	}

	/* IRC server *********************************************************** */
	while (true)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(irc.getMasterSocket(), &readfds);
		max_sd = irc.getMasterSocket();
		for (int i = 0; i < irc.getMaxClient(); i++)
		{
			sd = client[i].GetSocket();
			if (sd > 0)
			{
				FD_SET(sd, &readfds);
				FD_SET(sd, &writefds);
			}
			if (sd > max_sd)
				max_sd = sd;
		}
		activity = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
		if ((activity < 0) && (errno != EINTR))
			std::cerr << ERROR << "select(): " << strerror(errno) << ENDL;
		if (FD_ISSET(irc.getMasterSocket(), &readfds))
		{
			irc.setAccept();
			if (irc.getAccept() == -1)
			{
				std::cerr << ERROR << "accept(): " << strerror(errno) << ENDL;
				return (1);
			}
			std::cout << "New connection from fd : " << irc.getAccept() <<
			std::endl;
			for (int i = 0; i < irc.getMaxClient(); i++)
			{
				if (client[i].GetSocket() == 0)
				{
					client[i].SetSocket(irc.getAccept());
					break ;
				}
			}
		}
		for (int i = 0; i < irc.getMaxClient(); i++)
		{
			sd = client[i].GetSocket();
			if (FD_ISSET(sd, &readfds))
			{
				valread = read(sd, irc.getBuffer(), 1024);
				if (valread == 0)
				{
					std::cerr << RED << "Client disconnect." << ENDL;
					close(sd);
					client[i].SetSocket(0);
					client[i].setbvn(1);
					client[i].Setok(0);
				}
				else
				{
					irc.getBuffer()[valread] = 0;
					client[i].SetMessage(irc.getBuffer());
					test = client[i].GetMessage();
					std::cout << "DEBUG : buffer : " << irc.getBuffer() <<
					std::endl;
					if (startwith("CAP LS\r\n", test))
						test.erase(0, 8);
					else
						std::cerr << "Placeholder text" << std::endl;
					std::cout <<"debug : message : " << test << std::endl;
					if (startwith("NICK ", test))
					{
						test.erase(0, 5);
						name = test;
						test.erase(test.find("\r\n"));
						client[i].SetNick(test);
						std::cout << "DEBUG : client : " << i << " setnick to "
						<< test << std::endl;
						name.erase(0, test.length() + 2);
						test = name;
					}
					else
						std::cerr << "Placeholder text" << std::endl;
					if (startwith("USER ", test))
					{
						test.erase(0, 5);
						name = test;
						name.erase(name.find(" "));
						client[i].SetUserName(name);
						name = test;
						name.erase(name.find(" "));
						client[i].SetHostName(name);
						name = test;
						name.erase(name.find(" "));
						client[i].SetHost(name);
						name = test;
						name.erase(name.find(" :"));
						client[i].SetRealName(name);
						client[i].Setok(1);
						std::cout << "client id " << i << " ok" << std::endl;
					}
				}
			}
			if (FD_ISSET(sd, &writefds))
			{
				if (client[i].getok() && client[i].getbvn())
				{
					test = ":ircserver 001 ";
					test.append(client[i].getUserName());
					test.append(" :coucou");
					std::cout << "DEBUG bvn message : " << test << "    " <<
					client[i].GetSocket() << std::endl;
					write(client[i].GetSocket(), test.c_str(), test.length());
					client[i].setbvn(0);
				}
			}
		}
	}
	return (0);
}
