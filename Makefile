NAME	= ircserver
CC		= c++
DEP_FLAG= -MMD
FLAGS	= -Wall -Wextra -Werror ${C98} ${DEP_FLAG}
C98		= -std=c++98
INCL	= -I includes
SRCS	=	$(addsuffix .cpp,		\
			$(addprefix srcs/,		\
				main				\
			$(addprefix parsing/,	\
				parsing				\
			)						\
			$(addprefix error/,		\
				error				\
			)						\
			$(addprefix init/,		\
				init				\
			)))
CLAS	= 	$(addsuffix .cpp,		\
			$(addprefix classes/,	\
				User				\
			))
OBJS	= ${SRCS:.cpp=.o} ${CLAS:.cpp=.o}
DEPS	= ${OBJS:.o=.d}
RM		:= rm -rf
RED		:= \033[1;31m
NC		:= \033[0m
LGREEN	:= \033[1;32m
CYAN	:= \033[1;36m

%.o: %.cpp
	@echo "\033[1A                                                              "
	@echo -n "\033[1A"
	@echo "${CYAN}Compiling $< ${NC}"
	@${CC} ${FLAGS} -o $@ -c $< ${INCL}

${NAME}: ${OBJS}
	@${CC} ${FLAGS} ${OBJS} ${INCL} -o $@ 
	@echo "${LGREEN}Successfully created${NC}${CYAN} ${NAME}${NC}${LGREEN}!${NC}"

all: ${NAME}

test: re 
	@echo "${CYAN}No automated tests for this one!${NC}"
	@valgrind ./${NAME}
	@make fclean

clean:
	@echo "${CYAN}Cleaned ${NAME}${NC}"
	@${RM} ${OBJS} ${DEPS}

fclean:
	@echo "${CYAN}FCleaned ${NAME}${NC}"
	@${RM} ${OBJS} ${NAME} ${DEPS}

re: fclean all

-include ${DEPS}

.PHONY: all clean fclean re