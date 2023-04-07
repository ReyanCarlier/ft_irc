# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/22 11:58:06 by frrusso           #+#    #+#              #
#    Updated: 2023/04/07 15:12:19 by nfelsemb         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ircserv
SRC			=	main.cpp \
				Server.cpp \
				Client.cpp
CPP			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++98 -MMD
RM			=	rm -rf
INC			=	-I ./inc/
SRC_PATH	=	./src/
OBJ_PATH	=	./obj/
OBJ_DIRS	=	${OBJ_PATH}
OBJ			=	${addprefix ${OBJ_PATH},${SRC:.cpp=.o}}
DEPS		=	${addprefix ${OBJ_PATH},${SRC:.cpp=.d}}
GARBAGE		=	.vscode
PORT		=	6667
PASSWORD	=	password
PROGRESS	=	🔁
CHECK		=	✅

all:${NAME}

clean:
	@echo -n "\e[2K\r- \33[36mCleaning objects\33[0m [${PROGRESS}]"
	@${RM} ${OBJ_PATH}
	@echo -n "\e[2K\r- \33[31mCleaning objects\33[0m [${CHECK}]\n"

fclean:clean
	@echo -n "\e[2K\r- \33[36mCleaning execution file\33[0m [${PROGRESS}]"
	@${RM} ${NAME} ${GARBAGE}
	@echo -n "\e[2K\r- \33[31mCleaning execution file\33[0m [${CHECK}]\n"

re:fclean
	@make all

run:${NAME}
	clear
	./${NAME} ${PORT} ${PASSWORD}
	${RM} ${OBJ_PATH} ${NAME} ${GARBAGE}

debug:${NAME}
	clear
	valgrind --track-fds=yes ./${NAME} ${PORT} ${PASSWORD}
	${RM} ${OBJ_PATH} ${NAME} ${GARBAGE}

${OBJ_PATH}%.o:${SRC_PATH}%.cpp
	@echo -n "\e[2K\r- \33[1;34m$<\33[0m [${PROGRESS}]"
	@${CPP} ${CPPFLAGS} ${INC} -c $< -o $@
	@echo -n "\e[2K\r- \33[1;32m$<\33[0m [${CHECK}]\n"

${OBJ_DIRS}:
	@mkdir ${OBJ_DIRS}

${NAME}:${OBJ_DIRS} ${OBJ}
	@echo -n "\e[2K\r- Build \33[1;33m${NAME}\33[0m [${PROGRESS}]"
	@${CPP} ${OBJ} -o $@
	@echo -n "\e[2K\r- Build \33[1;32m${NAME}\33[0m [${CHECK}]\n"

.PHONY:all clean fclean re run debug

-include ${DEPS}
