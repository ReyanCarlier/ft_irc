/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:01:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/12 14:52:44 by recarlie         ###   ########.fr       */
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
		std::cout << YELLOW << "Usage: ./serverserv <port> <password>" << ENDL;
		return (0);
	}
	if (isport(av[1]) == false)
	{
		std::cerr << YELLOW << "Port \"" << av[1] << "\" is not good." << ENDL;
		return (0);
	}

	/* Variable ************************************************************* */
	Server	server;
	fd_set readfds;
	fd_set writefds;
	try
	{
		server.run();
		std::cout << GREEN << "✅ Socket created successfully." << ENDL;
		server.bind(atoi(av[1]));
		std::cout << GREEN << "✅ Socket binded successfully." << ENDL;
		server.listen();
		std::cout << GREEN << "✅ Server listening on 127.0.0.1:" <<
		server.getPort() << "." << ENDL;
	
		//////////////////////// MAIN LOOP  ////////////////////////

		int			sd, valread;
		std::string	test, name;
		Client		*currentClient = NULL;
		/* IRC server ******************************************************* */
		while (42)
		{
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			int new_socket;
			struct sockaddr_in address;
			int activity, max_sd = 0;
			int addrlen = sizeof(address);
			
			FD_SET(server.getSocketFd(), &readfds);
			FD_SET(server.getSocketFd(), &writefds);
			max_sd = server.getSocketFd();

			/// NOT READ IF NO CLIENT
			for (size_t i = 0; i < server.getClients().size(); i++)
			{
				sd = server.getClients().at(i)->getSocket();
				if(sd > 0)
				{
					FD_SET(sd, &readfds);
					FD_SET(sd, &writefds);
				}
				if(sd > max_sd)
					max_sd = sd;
			}

			activity = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
			if((activity < 0) && (errno!=EINTR))
				std::cerr << "select error" << std::endl;

			if (FD_ISSET(server.getSocketFd(), &readfds))
			{
				new_socket = accept(
					server.getSocketFd(),
					reinterpret_cast<sockaddr*>(&address),
					reinterpret_cast<socklen_t*>(&addrlen)
				);
				if (new_socket < 0)
					throw "Failed to accept connection.";

				std::cout << GREEN << "✅ Connection accepted on FD " << new_socket << "." << ENDL;
				try
				{
					currentClient = server.getClient(new_socket);
					std::cout << "COUCOU" << std::endl;
				}
				catch (const char *e)
				{
					Client *newClient = new Client(new_socket);
					server.addClient(newClient);
					std::cout << "DEBUG : currentClient->getSocket() : " << server.getClient(new_socket)->getSocket() << std::endl;
					currentClient = newClient;
				}

				sd = currentClient->getSocket();
				std::cout << YELLOW << "DEBUG : sd avant valread : " << sd << ENDL;
				valread = read(sd, server.getBuffer(), 1024);
				std::cout << "DEBUG : valread : " << valread << std::endl;
				if (valread == 0)
				{
					std::cerr << RED << "Client disconnect." << ENDL;
					close(sd);
					server.removeClient(currentClient);
				}
				else
				{
					server.getBuffer()[valread] = '\0';
					currentClient->setMessage(server.getBuffer());
					test = currentClient->getMessage();
					std::cout << YELLOW << "DEBUG : buffer : " << server.getBuffer() << ENDL;

					if (startwith("CAP LS\r\n", test))
						test.erase(0, 8);
					else
						std::cerr << "Placeholder text" << std::endl;
					if (startwith("NICK ", test))
					{
						test.erase(0, 5);
						name = test;
						test.erase(test.find("\r\n"));
						currentClient->setNick(test);
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
						currentClient->setUserName(name);
						name = test;
						name.erase(name.find(" "));
						currentClient->setHostName(name);
						name = test;
						name.erase(name.find(" "));
						currentClient->setHost(name);
						name = test;
						name.erase(name.find(" :"));
						currentClient->setRealName(name);
						currentClient->setOk(1);
					}
				}
			}

			for (size_t i = 0; i < server.getClients().size(); i++)
			{
				currentClient = server.getClients().at(i);
				sd = currentClient->getSocket();
				if (FD_ISSET(sd, &writefds))
				{
					if (currentClient->getok() && currentClient->getbvn())
					{
						test = ":serverserver 001 ";
						test.append(currentClient->getUserName());
						test.append(" :coucou\r\n");
						std::cout << "DEBUG bvn message : " << test << "    " <<
						currentClient->getSocket() << std::endl;
						write(currentClient->getSocket(), test.c_str(),
						test.size());
						currentClient->setbvn(0);
					}
				}
			}
			// auto var = FD_ISSET(server.getSocketFd(), &writefds);
			// std::cout << "Retour de FD_ISSET : " << var << std::endl;
			// if (var)
			// {
			// 	std::cout << " la" << ENDL;
				// if (currentClient->getok() && currentClient->getbvn())
				// {
				// 	test = ":serverserver 001 ";
				// 	test.append(currentClient->getUserName());
				// 	test.append(" :coucou\r\n");
				// 	std::cout << "DEBUG bvn message : " << test << "    " <<
				// 	currentClient->getSocket() << std::endl;
				// 	write(currentClient->getSocket(), test.c_str(),
				// 	test.size());
				// 	currentClient->setbvn(0);
				// }
			// }
		}
		//////////////////////// MAIN LOOP  ////////////////////////
	}
	catch(const char *e)
	{
		std::cerr << RED << e << ENDL;
		return (1);
	}
	return (0);
}
