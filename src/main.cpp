/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:01:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/03/22 12:01:33 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

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
	int		opt = 1;

	/* IRC server setup ***************************************************** */
	if (irc.getSocket() == -1)
	{
		std::cerr << ERROR << "socket(): " << strerror(errno) << ENDL;
		return (1);
	}
	if (setsockopt(irc.getSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(opt)))
	{
		std::cerr << ERROR << "setsockopt(): " << strerror(errno) << ENDL;
		return (1);
	}
	irc.setAddress();
	if (bind(irc.getSocket(), irc.getCastAddress(), sizeof(sockaddr_in)) == -1)
	{
		std::cerr << ERROR << "bind(): " << strerror(errno) << ENDL;
		return (1);
	}
	if (listen(irc.getSocket(), MAX_REQUESTS) == -1)
	{
		std::cerr << ERROR << "listen(): " << strerror(errno) << ENDL;
		return (1);
	}

	/* IRC server *********************************************************** */
	for (int i = 0; i < 3; i++)
	{
		irc.setAccept();
		if (irc.getAccept() == -1)
		{
			std::cerr << ERROR << "accept(): " << strerror(errno) << ENDL;
			return (1);
		}
		read(irc.getAccept(), irc.getBuffer(), 1024);
		std::cout << irc.getBuffer() << std::endl;
		send(irc.getAccept(), "Hello from server.", 18, 0);
		std::cout << BLUE << "Hello message sent." << ENDL;

		/* Close server ***************************************************** */
		close(irc.getAccept());
	}
	return (0);
}
