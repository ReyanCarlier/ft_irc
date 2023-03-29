# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: frrusso <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/22 11:58:06 by frrusso           #+#    #+#              #
#    Updated: 2023/03/22 11:58:08 by frrusso          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ircserv
SRC			=	main.cpp
CPP			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++98 -MMD
RM			=	rm -rf
INC			=	-I ./inc/
SRC_PATH	=	./src/
OBJ_PATH	=	./obj/
OBJ_DIRS	=	${OBJ_PATH}
OBJ			=	${addprefix ${OBJ_PATH},${SRC:.cpp=.o}}
DEPS		=	${addprefix ${OBJ_PATH},${SRC:.cpp=.d}}
GARBAGE		=	.vscode client
PORT		=	8080
PASSWORD	=	""

all:${NAME}

clean:
	${RM} ${OBJ_PATH}

fclean:clean
	${RM} ${NAME} ${GARBAGE}

re:fclean
	make all

run:${NAME}
	${CPP} -Wall -Wextra -Werror -std=c++98 src/client.cpp -o client
	clear
	./${NAME} ${PORT} ${PASSWORD}
	${RM} ${OBJ_PATH} ${NAME} ${GARBAGE}

debug:${NAME}
	${CPP} -Wall -Wextra -Werror -std=c++98 src/client.cpp -o client
	clear
	valgrind --track-fds=yes ./${NAME} ${PORT} ${PASSWORD}
	${RM} ${OBJ_PATH} ${NAME} ${GARBAGE}

${OBJ_PATH}%.o:${SRC_PATH}%.cpp
	${CPP} ${CPPFLAGS} ${INC} -c $< -o $@

${OBJ_DIRS}:
	mkdir ${OBJ_DIRS}

${NAME}:${OBJ_DIRS} ${OBJ}
	${CPP} ${OBJ} -o $@

.PHONY:all clean fclean re run debug

-include ${DEPS}
