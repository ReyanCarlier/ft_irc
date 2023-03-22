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

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << YELLOW << "Usage: ./ircserv <port> <password>" << ENDL;
		return (0);
	}

	// Debug
	(void)ac;
	(void)av;
	for (int i = 0; i < 32; i++)
	{
		std::cout << RED << "Ar" << YELLOW << "c-" << GREEN << "en" << CYAN <<
		"-c" << BLUE << "ie" << MAGENTA << "l!" << ENDL;
	}

	return (0);
}
