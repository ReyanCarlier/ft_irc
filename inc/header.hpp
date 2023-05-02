/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: recarlie <recarlie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 12:00:30 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/24 11:07:20 by recarlie         ###   ########.fr       */
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
#include <ctime>		/* time */
#include <sstream>		/* stringstream */
#include <vector>

/* Color */
#define RESET			"\33[00m"
#define BLACK			"\33[30m"
#define RED				"\33[31m"
#define GREEN			"\33[32m"
#define YELLOW			"\33[33m"
#define BLUE			"\33[34m"
#define MAGENTA			"\33[35m"
#define CYAN			"\33[36m"
#define ENDL			"\33[00m" << endl
#define ERROR			"\33[31m" << "Error: "

/* Server */
#define BUFFER_SIZE		1024
#define MAX_TCP_PORT	65535
#define MAX_CLIENT		42
#define MAX_IN_QUEUE	100
#define OPER_PASSWORD	"oper"

/* Bot */
#define BOT_NAME		"bot"
#define BOT_PASSWORD	"password"

/* Code */
#define CLIENTS			server.getClients().at(i)
#define LINE			"\n===================================================="
#define CL_IT			vector<Client*>::iterator it
#define CL_IT2			vector<Client*>::iterator it2
#define CH_IT			vector<Channel*>::iterator it

/* Namespace */
using namespace std;
