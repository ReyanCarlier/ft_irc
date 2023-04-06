/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:01:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/06 16:43:09 by nfelsemb         ###   ########.fr       */
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
	fd_set readfds;
	int		opt = 1;
	int		max_sd, sd, activity, valread;
	int		client_socket[irc.getMaxClient()];
	/* IRC server setup ***************************************************** */
	for (int i = 0 ; i < irc.getMaxClient(); i++)
	{
		client_socket[i] = 0;
	}
	if (irc.getMasterSocket() == -1)
	{
		std::cerr << ERROR << "socket(): " << strerror(errno) << ENDL;
		return (1);
	}
	if (setsockopt(irc.getMasterSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(int)))
	{
		std::cerr << ERROR << "setsockopt(): " << strerror(errno) << ENDL;
		return (1);
	}
	irc.setAddress();
	if (bind(irc.getMasterSocket(), irc.getCastAddress(), sizeof(sockaddr_in)) == -1)
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
	while(42)
	{
		FD_ZERO(&readfds);
		
		FD_SET(irc.getMasterSocket(), &readfds);
		max_sd = irc.getMasterSocket();
		for (int i = 0; i < irc.getMaxClient(); i++)
		{
			sd = client_socket[i];
			
			if(sd > 0)
				FD_SET(sd, &readfds);
			if(sd > max_sd)
				max_sd = sd;
		}
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		
		if((activity < 0) && (errno!=EINTR))
			std::cerr << "select error" << std::endl;

		
		if (FD_ISSET(irc.getMasterSocket(), &readfds))
		{
			irc.setAccept();
			if(irc.getAccept() == -1)
			{
				std::cerr << "accept error" << std::endl;
				return (1);
			}
			std::cout << "New connection from fd : " << irc.getAccept() << std::endl;
			if (send(irc.getAccept(), "Coucou", 6, 0) != 6)
				std::cerr << "send error" << std::endl;
			std::cout << "Coucou send" << std::endl;
			for(int i = 0; i < irc.getMaxClient(); i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = irc.getAccept();
					break ;
				}
			}
		}
		
		for(int i = 0; i < irc.getMaxClient(); i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				valread = read(sd, irc.getBuffer(), 1024);
				if(valread == 0)
				{
					std::cerr << "Host disconnect" << std::endl;
					close (sd);
					client_socket[i] = 0;
				}
				else
				{
					irc.getBuffer()[valread] = 0;
					send(sd, irc.getBuffer(), strlen(irc.getBuffer()), 0);
					std::cout << irc.getBuffer() << std::endl;
				}
			}
		}
	}
	return (0);
}