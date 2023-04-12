/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:01:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/11 18:41:49 by recarlie         ###   ########.fr       */
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
		while (true)
		{
			int new_socket;
			struct sockaddr_in address;
			int addrlen = sizeof(address);
			
			std::cout << RED << "Waiting for new connection..." << ENDL;
			new_socket = accept(
				server.getSocketFd(),
				reinterpret_cast<sockaddr*>(&address),
				reinterpret_cast<socklen_t*>(&addrlen)
			);
			if (new_socket < 0)
				throw "Failed to accept connection.";
			std::cout << GREEN << "✅ Connection accepted on FD " << new_socket
			<< "." << ENDL;
			// Loop for each _clients list from server
			if (server.getClients().size() == 0)
			{
				currentClient = new Client();
				currentClient->setSocket(new_socket);
				server.addClient(*currentClient);
			}
			else if ((currentClient = server.getClient(new_socket)) == NULL)
			{
				currentClient = new Client();
				currentClient->setSocket(new_socket);
				server.addClient(*currentClient);
			}
			else
			{
				std::cout << "LE CLIENT EXISTE TA GROSSE TETE" << ENDL;
			}
			
			sd = currentClient->getSocket();
			valread = read(sd, server.getBuffer(), 1024);
			if (valread == 0)
			{
				std::cerr << RED << "Client disconnect." << ENDL;
				close(sd);
				currentClient->setSocket(0);
				currentClient->setbvn(1);
				currentClient->setOk(0);
			}
			else
			{
				server.getBuffer()[valread] = 0;
				currentClient->setMessage(server.getBuffer());
				test = currentClient->getMessage();
				std::cout << "DEBUG : buffer : " << server.getBuffer() <<
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
					currentClient->setNick(test);
					std::cout << "DEBUG : client : " << currentClient->getUserName() << " setNick to " << test << std::endl;
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
					currentClient->setRealName(name);
					name = test;
					name.erase(name.find(" :"));
					currentClient->setRealName(name);
					currentClient->setOk(1);
					std::cout << "client id " << currentClient->getUserName() <<
					" ok" << std::endl;
				}
				if (currentClient->getok() && currentClient->getbvn())
				{
					test = ":serverserver 001 ";
					test.append(currentClient->getUserName());
					test.append(" :coucou\r\n");
					std::cout << "DEBUG bvn message : " << test << "    " <<
					currentClient->getSocket() << std::endl;
					write(currentClient->getSocket(), test.c_str(), test.size());
					currentClient->setbvn(0);
				}
			}
			// for (int i = 0; i < MAX_CLIENT; i++)
			// {
				
			// 	Client *currentClient = server.getClient(i);
				
				
			// 	if (FD_ISSET(sd, &writefds))
			// 	{
			// 		if (currentClient->getok() && currentClient->getbvn())
			// 		{
			// 			test = ":serverserver 001 ";
			// 			test.append(currentClient->getUserName());
			// 			test.append(" :coucou\r\n");
			// 			std::cout << "DEBUG bvn message : " << test << "    " <<
			// 			currentClient->getSocket() << std::endl;
			// 			write(currentClient->getSocket(), test.c_str(), test.length());
			// 			currentClient->setbvn(0);
			// 		}
			// 	}
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
