/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 17:09:13 by frrusso           #+#    #+#             */
/*   Updated: 2023/03/22 17:09:14 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <header.hpp>
#include <map>

class Server
{
	private:
		std::map<std::string,std::string>	_user;
	public:
		Server(void);
		~Server();
};

#endif
