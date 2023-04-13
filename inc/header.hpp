/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:00:30 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/13 11:41:57 by recarlie         ###   ########.fr       */
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

