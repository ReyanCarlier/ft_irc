/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:01:31 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/24 15:56:23 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

bool	is_port(char *av) {
	if (strlen(av) > 5)
		return (false);
	for (size_t i = 0; i < strlen(av); i++) {
		if (isdigit(av[i]) == false)
			return (false);
	}
	if (atoi(av) > MAX_TCP_PORT)
		return (false);
	return (true);
}

int	main(int ac, char **av)
{
	if (ac != 3) {
		cout << RED << "Usage: ./serverserv <port> <password>" << ENDL;
		return (0);
	}
	if (is_port(av[1]) == false) {
		cerr << RED << "Port \"" << av[1] << "\" is not good." << ENDL;
		return (0);
	}

	Server	server(av);
	fd_set	readfds;
	fd_set	writefds;

	try {
		server.run();
		cout << GREEN << "✅ Socket created successfully." << ENDL;
		server.bind(atoi(av[1]));
		cout << GREEN << "✅ Socket binded successfully." << ENDL;
		server.listen();
		cout << GREEN << "✅ Server listening on 127.0.0.1:" <<
		server.getPort() << "." << ENDL;

		int			valread;
		string	buffer, name;

		while (server.getDie() == false)  {
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_SET(server.getSocketFd(), &readfds);
			FD_SET(server.getSocketFd(), &writefds);
			if (select(server.getHighestFd(&readfds, &writefds) + 1, &readfds, &writefds, NULL, NULL) < 0 && errno != EINTR)
				cerr << "select error" << endl;
			if (FD_ISSET(server.getSocketFd(), &readfds)) {
				sockaddr_in	address;
				int			addrlen = sizeof(address);
				int			new_socket = accept(
					server.getSocketFd(),
					reinterpret_cast<sockaddr*>(&address),
					reinterpret_cast<socklen_t*>(&addrlen)
				);

				if (new_socket < 0)
					throw "Failed to accept connection.";
				cout << CYAN << "✅ Connection accepted on FD " << new_socket <<
				". client in queue " << server.getClientQueue() << ENDL;
				if (
					server.getClientQueue() >= MAX_IN_QUEUE ||
					server.getClients().size() >= MAX_CLIENT
				) {
					write(new_socket, ":serverserver ERROR: Server is full.\r\n", 38);
					close(new_socket);
					continue ;
				}
				server.addClientQueue();
				try {
					server.getClient(new_socket);
				} catch (const char *e) {
					server.addClient(new Client(new_socket));
				}
			}
			for (size_t i = 0; i < server.getClients().size(); i++) {
				if (FD_ISSET(CLIENTS->getSocket(), &writefds)) {
					if (
						CLIENTS->isReady() &&
						CLIENTS->isWelcomed() &&
						!CLIENTS->getNickname().empty() &&
						(CLIENTS->getPass() == 1 || server.getPassword().empty())
					) {
						server.welcome(CLIENTS);
						server.removeClientQueue();
					} else if (
						CLIENTS->getPass() == 0 && !server.getPassword().empty()
					) {
						server.sendToClient(
							":serverserver 464 " + CLIENTS->getUsername() + " :Password incorrect",
							CLIENTS
						);
						close(CLIENTS->getSocket());
						server.removeClient(CLIENTS);
						continue ;
					} else if (
						CLIENTS->isReady() == 1 &&
						CLIENTS->isWelcomed() &&
						CLIENTS->getPass() == 2 &&
						!server.getPassword().empty()
					) {
						server.sendToClient(
							":serverserver 461 " + CLIENTS->getUsername() + " PASS :Not enough parameters",
							CLIENTS
						);
						close(CLIENTS->getSocket());
						server.removeClient(CLIENTS);
						continue ;
					}
				}
				if (FD_ISSET(CLIENTS->getSocket(), &readfds))
				{
					valread = read(CLIENTS->getSocket(), server.getBuffer(), BUFFER_SIZE);
					if (valread == 0)
					{
						cerr << RED << "Client " << CLIENTS->getSocket() << " disconnected." << ENDL;
						if (CLIENTS->isWelcomed())
							server.removeClientQueue();
						close(CLIENTS->getSocket());

						vector<Channel*>	channels = server.getChannels();

						for (size_t j = 0; j < channels.size(); j++)
						{
							vector<Client*>	clients = channels.at(j)->getClients();
							for (size_t k = 0; k < clients.size(); k++)
							{
								if (clients.at(k)->getSocket() == CLIENTS->getSocket())
								{
									channels.at(j)->removeClient(CLIENTS);
									break ;
								}
							}
							if (channels.at(j)->getClients().size() == 0)
								server.removeChannel(channels.at(j));
						}
						server.removeClient(CLIENTS);
					} else {
						server.getBuffer()[valread] = '\0';
						server.commandHandler(server.getBuffer(), CLIENTS);
					}
				}
			}
		}
	} catch(const char *e) {
		cerr << RED << e << ENDL;
		return (1);
	}
	return (0);
}
