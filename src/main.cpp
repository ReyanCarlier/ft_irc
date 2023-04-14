/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:01:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/14 13:30:14 by nfelsemb         ###   ########.fr       */
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
	if (ac != 3)
	{
		std::cout << RED << "Usage: ./serverserv <port> <password>" << ENDL;
		return (0);
	}
	if (isport(av[1]) == false)
	{
		std::cerr << RED << "Port \"" << av[1] << "\" is not good." << ENDL;
		return (0);
	}

	Server	server(av);
	fd_set	readfds;
	fd_set	writefds;

	try
	{
		server.run();
		std::cout << GREEN << "✅ Socket created successfully." << ENDL;
		server.bind(atoi(av[1]));
		std::cout << GREEN << "✅ Socket binded successfully." << ENDL;
		server.listen();
		std::cout << GREEN << "✅ Server listening on 127.0.0.1:" <<
		server.getPort() << "." << ENDL;

		int			valread;
		std::string	buffer, name;

		while (server.getDie() == false)
		{
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_SET(server.getSocketFd(), &readfds);
			FD_SET(server.getSocketFd(), &writefds);

			if (select(server.getHighestFd(&readfds, &writefds) + 1, &readfds, &writefds, NULL, NULL) < 0 && errno != EINTR)
				std::cerr << "select error" << std::endl;

			if (FD_ISSET(server.getSocketFd(), &readfds))
			{
				struct sockaddr_in address;
				int addrlen = sizeof(address);
				
				int new_socket = accept(
					server.getSocketFd(),
					reinterpret_cast<sockaddr*>(&address),
					reinterpret_cast<socklen_t*>(&addrlen)
				);
				if (new_socket < 0)
					throw "Failed to accept connection.";

				std::cout << CYAN << "✅ Connection accepted on FD " << new_socket << "." << ENDL;
				try
				{
					server.getClient(new_socket);
				}
				catch (const char *e)
				{
					server.addClient(new Client(new_socket));
				}
			}

			for (size_t i = 0; i < server.getClients().size(); i++)
			{
				if (FD_ISSET(server.getClients().at(i)->getSocket(), &writefds))
				{
					if (
						server.getClients().at(i)->isReady() &&
						server.getClients().at(i)->isWelcomed() &&
						(server.getClients().at(i)->getPass() == 1 || server.getPassword().empty())
					)
					{
						server.welcome(server.getClients().at(i));
					}
					else if (server.getClients().at(i)->getPass() == 0 && !server.getPassword().empty())
					{
						std::string test = ":serverserver 464 ";
						test.append(server.getClients().at(i)->getUsername());
						test.append(" :Password incorrect\r\n");
						std::cout << "DEBUG bad pass message : " << test << "    " <<
						server.getClients().at(i)->getSocket() << std::endl;
						write(server.getClients().at(i)->getSocket(), test.c_str(),test.size());
						close(server.getClients().at(i)->getSocket());
						server.removeClient(server.getClients().at(i));
						continue ;
					}
					else if (
						server.getClients().at(i)->isReady() == 1 &&
						server.getClients().at(i)->isWelcomed() &&
						server.getClients().at(i)->getPass() == 2 &&
						!server.getPassword().empty()
					)
					{
						std::string test = ":serverserver 461 ";
						test.append(server.getClients().at(i)->getUsername());
						test.append(" PASS :Not enough parameters\r\n");
						std::cout << "DEBUG bad pass message : " << test << "    " <<
						server.getClients().at(i)->getSocket() << std::endl;
						write(server.getClients().at(i)->getSocket(), test.c_str(),test.size());
						close(server.getClients().at(i)->getSocket());
						server.removeClient(server.getClients().at(i));
						continue ;
					}
				}
				if (FD_ISSET(server.getClients().at(i)->getSocket(), &readfds))
				{
					valread = read(server.getClients().at(i)->getSocket(), server.getBuffer(), 1024);
					if (valread == 0)
					{
						std::cerr << RED << "Client " << server.getClients().at(i)->getSocket() << " disconnected." << ENDL;
						close(server.getClients().at(i)->getSocket());

						std::vector<Channel*> channels = server.getChannels();
						for (size_t j = 0; j < channels.size(); j++)
						{
							std::vector<Client*> clients = channels.at(j)->getClients();
							for (size_t k = 0; k < clients.size(); k++)
							{
								if (clients.at(k)->getSocket() == server.getClients().at(i)->getSocket())
								{
									channels.at(j)->removeClient(server.getClients().at(i));
									break ;
								}
							}
							if (channels.at(j)->getClients().size() == 0)
							{
								std::cout << RED << "Channel " << channels.at(j)->getName() << " deleted." << ENDL;
								server.removeChannel(channels.at(j));
								delete channels.at(j);
							}
						}
						Client *client = server.getClients().at(i);
						server.removeClient(server.getClients().at(i));
						delete client;
					}
					else
					{
						server.getBuffer()[valread] = '\0';
						server.commandHandler(server.getBuffer(), server.getClients().at(i));
					}
				}
			}
		}
	}
	catch(const char *e)
	{
		std::cerr << RED << e << ENDL;
		return (1);
	}
	return (0);
}
