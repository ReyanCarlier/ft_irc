/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:00:30 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/13 11:22:58 by recarlie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>	// socket setsockopt getsockname bind
						// connect listen accept send recv
#include <netdb.h>		// getprotobyname gethostbyname getaddrinfo freeaddrinfo
#include <arpa/inet.h>	// htons htonl ntohs ntohl inet_addr inet_ntoa
#include <csignal>		// signal
#include <unistd.h>		// lseek
#include <sys/stat.h>	// fstat
#include <fcntl.h>		// fcntl
#include <poll.h>		// poll
#include <cstring>		// strlen
#include <cstdlib>		// atoi
#include <cerrno>		// errno

#define RESET			"\33[00m"
#define BLACK			"\33[30m"
#define RED				"\33[31m"
#define GREEN			"\33[32m"
#define YELLOW			"\33[33m"
#define BLUE			"\33[34m"
#define MAGENTA			"\33[35m"
#define CYAN			"\33[36m"
#define ENDL			"\33[00m" << std::endl
#define ERROR			"\33[31m" << "Error: "
#define MAX_TCP_PORT	65535
#define MAX_REQUESTS	8
#define MAX_CLIENT		42
#define SERVER_NAME     "ft_irc"

#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_NORECIPIENT "411"
#define ERR_NOTEXTTOSEND "412"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NOMOTD "422"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433"
#define ERR_NOTONCHANNEL "442"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTREGISTERED "451"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_YOUREBANNEDCREEP "465"
#define ERR_KEYSET "467"
#define ERR_CHANNELISFULL "471"
#define ERR_UNKNOWNMODE "472"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BANNEDFROMCHAN "474"
#define ERR_BADCHANNELKEY "475"
#define ERR_NOPRIVILEGES "481"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_NOOPERHOST "491"
#define ERR_UMODEUNKNOWNFLAG "501"
#define ERR_USERSDONTMATCH "502"