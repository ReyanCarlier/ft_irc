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

#include <header.hpp>

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
	int					server_fd = socket(AF_INET, SOCK_STREAM, 0);
	int					opt = 1;
	int					port = atoi(av[1]);
	std::string			password(av[2]);
	struct sockaddr_in	address;
	int					addrlen = sizeof(address);
	int					new_socket;
	char				buffer[1024] = {0};

	/* IRC server setup ***************************************************** */
	if (server_fd == -1)
	{
		std::cerr << ERROR << "socket(): " << strerror(errno) << ENDL;
		close(server_fd);
		return (1);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
	sizeof(opt)))
	{
		std::cerr << ERROR << "setsockopt(): " << strerror(errno) << ENDL;
		close(server_fd);
		return (1);
	}
	memset(reinterpret_cast<char*>(&address), 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);
	if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&address),
	sizeof(address)) == -1)
	{
		std::cerr << ERROR << "bind(): " << strerror(errno) << ENDL;
		close(server_fd);
		return (1);
	}
	if (listen(server_fd, MAX_REQUESTS) == -1)
	{
		std::cerr << ERROR << "listen(): " << strerror(errno) << ENDL;
		close(server_fd);
		return (1);
	}

	/* IRC server *********************************************************** */
	for (int i = 0; i < 8; i++)
	{
		new_socket = accept(
			server_fd,
			reinterpret_cast<struct sockaddr*>(&address),
			reinterpret_cast<socklen_t*>(&addrlen)
		);
		if (new_socket == -1)
		{
			std::cerr << ERROR << "accept(): " << strerror(errno) << ENDL;
			close(server_fd);
			return (1);
		}
		read(new_socket, buffer, 1024);
		std::cout << buffer << std::endl;
		send(new_socket, "Hello from server.", 18, 0);
		std::cout << BLUE << "Hello message sent." << ENDL;

		/* Close server ***************************************************** */
		close(new_socket);
	}
	close(server_fd);
	return (0);
}
