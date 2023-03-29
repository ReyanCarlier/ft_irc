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
	if (ac != 3)
	{
		std::cout << YELLOW << "Usage: ./ircserv <port> <password>" << ENDL;
		return (0);
	}
	if (isport(av[1]) == false)
	{
		std::cerr << ERROR << "Port \"" << av[1] << "\" is not good." << ENDL;
		return (1);
	}

	int	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_fd == -1)
	{
		std::cerr << ERROR << "Cannot create socket." << ENDL;
		return (1);
	}

	int					port = atoi(av[1]);
	std::string			password(av[2]);
	struct sockaddr_in	address;

	memset(reinterpret_cast<char*>(&address), 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);
	if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&address),
	sizeof(address)) == -1)
	{
		std::cerr << ERROR << "bind() failed." << ENDL;
		return (1);
	}
	if (listen(server_fd, MAX_REQUESTS) == -1)
	{
		std::cerr << ERROR << "listen() failed." << ENDL;
		return (1);
	}

	int	addrlen = sizeof(address);
	int	new_socket = accept(
		server_fd,
		reinterpret_cast<struct sockaddr*>(&address),
		reinterpret_cast<socklen_t*>(&addrlen)
	);

	if (new_socket == -1)
	{
		std::cerr << ERROR << "accept() failed." << ENDL;
		return (1);
	}

	char	buffer[1024] = {0};

	read(new_socket, buffer, 1024);
	std::cout << buffer << std::endl;
	send(new_socket, "Hello from server.", 18, 0);
	std::cout << BLUE << "Hello message sent." << ENDL;
	close(new_socket);
	shutdown(server_fd, SHUT_RDWR);
	close(server_fd);
	return (0);
}
