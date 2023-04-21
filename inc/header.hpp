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

/* Include */
#include <iostream>
#include <sys/socket.h>	/* socket setsockopt bind listen accept send */
#include <arpa/inet.h>	/* htons htonl */
#include <unistd.h>		/* close read write */
#include <cstring>		/* strlen */
#include <cstdlib>		/* atoi */
#include <cerrno>		/* errno */
#include <sstream>      /* stringstream */

/* Color */
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

/* Server */
#define MAX_TCP_PORT	65535
#define MAX_CLIENT		42
#define MAX_IN_QUEUE	2
#define BUFFER_SIZE		1024
#define OPER_PASSWORD	"oper"

/* Bot */
#define BOT_NAME		"bot"
#define BOT_PASSWORD	"password"
#define CMD_CMP			cmd.compare
