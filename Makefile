# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nfelsemb <nfelsemb@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/22 11:58:06 by frrusso           #+#    #+#              #
#    Updated: 2023/04/05 15:40:58 by nfelsemb         ###   ########.fr        #
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

PROGRESS = 🔁
CHECK = ✅

all:${NAME}

clean:
	@echo -n "\e[2K\r- \033[36mClean des objs\033[0m [${PROGRESS}]"
	@${RM} ${OBJ_PATH}
	@echo -n "\e[2K\r- \033[31mClean des objs\033[0m [${CHECK}]\n"

fclean:clean
	@echo -n "\e[2K\r- \033[36mClean de lexecutable\033[0m [${PROGRESS}]"
	@${RM} ${NAME} ${GARBAGE}
	@echo -n "\e[2K\r- \033[31mClean de lexecutable\033[0m [${CHECK}]\n"

re:fclean
	@make all

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
	@echo -n "\e[2K\r- \033[1;34m$<\033[0m [${PROGRESS}]"
	@${CPP} ${CPPFLAGS} ${INC} -c $< -o $@
	@echo -n "\e[2K\r- \033[1;32m$<\033[0m [${CHECK}]\n"

${OBJ_DIRS}:
	@mkdir ${OBJ_DIRS}

${NAME}:${OBJ_DIRS} ${OBJ}
	@echo -n "\e[2K\r- Build \033[1;33m${NAME}\033[0m [${PROGRESS}]"
	@${CPP} ${OBJ} -o $@
	@echo -n "\e[2K\r- Build \033[1;32m${NAME}\033[0m [${CHECK}]\n"

.PHONY:all clean fclean re run debug

-include ${DEPS}
