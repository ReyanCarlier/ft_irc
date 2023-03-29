/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 17:02:00 by frrusso           #+#    #+#             */
/*   Updated: 2023/03/28 17:09:44 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/header.hpp"

int	main(void)
{
	int					client_fd = socket(AF_INET, SOCK_STREAM, 0);
	char				hello[] = "Hello from client.";
	char				buffer[1024] = {0};
	struct sockaddr_in	serv_addr;

	if (client_fd == -1)
	{
		std::cerr << ERROR << "Socket creation failed." << ENDL;
		return (1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080);
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) == -1)
	{
		std::cerr << ERROR << "Invalid address." << ENDL;
		return (1);
	}
	if (connect(client_fd, reinterpret_cast<struct sockaddr*>(&serv_addr),
	sizeof(serv_addr)) == -1)
	{
		std::cerr << ERROR << "Connection Failed." << ENDL;
		return (1);
	}
	send(client_fd, hello, strlen(hello), 0);
	std::cout << BLUE << "Hello message sent." << ENDL;
	read(client_fd, buffer, 1024);
	std::cout << buffer << std::endl;
	close(client_fd);
	return (0);
}
