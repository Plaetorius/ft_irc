NAME	= ircserv
CC		= c++
DEP_FLAG= -MMD
FLAGS	= -Wall -Wextra ${C98} ${DEP_FLAG} -g3 #-----------------ADD WERROR
C98		= -std=c++98
INCL	= -I includes
SRCS	=	$(addsuffix .cpp,		\
			$(addprefix srcs/,		\
				main				\
			$(addprefix parsing/,	\
				parsing				\
				input				\
			)						\
			$(addprefix error/,		\
				error				\
			)						\
			$(addprefix init/,		\
				init				\
			)						\
			$(addprefix server/,	\
				actions				\
			)						\
			$(addprefix cleaning/,	\
				clear_data			\
			)						\
			$(addprefix utils/,		\
				find_user_fd		\
				int_to_string		\
				trim_spaces			\
				nick_to_fd			\
			)))
CLAS	= 	$(addsuffix .cpp,		\
			$(addprefix classes/,	\
				User				\
				Channel				\
			))
BONUS	= 	$(addsuffix .cpp,		\
			$(addprefix bot/,		\
				bot					\
			)						\
			$(addprefix utils/,		\
				trim_spaces			\
				int_to_string		\
			)						\
			$(addprefix error/,		\
				error_str			\
			)						\
			$(addprefix parsing/,	\
				parsing				\
			))
OBJS	= ${SRCS:.cpp=.o} ${CLAS:.cpp=.o}
BOBJS	= ${BONUS:.cpp=.o} ${CLAS:.cpp=.o}
DEPS	= ${OBJS:.o=.d}
BDEPS	= ${BOBJS:.o=.d}
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
	@echo "${LGREEN}Successfully created ${NC}${CYAN}${NAME}${NC}${LGREEN}!${NC}"

all: ${NAME}

bonus: ${BOBJS}
	@${CC} ${FLAGS} ${BOBJS} ${INCL} -o $@ 
	@echo "${LGREEN}Successfully created ${NC}${CYAN}Tosser${NC}${LGREEN}!${NC}"

test: re 
	@make fclean

clean:
	@echo "${CYAN}Cleaned ${NAME}${NC}"
	@${RM} ${OBJS} ${DEPS}

fclean:
	@echo "${CYAN}FCleaned ${NAME}${NC}"
	@${RM} ${OBJS} ${NAME} ${DEPS}

re: fclean all

-include ${DEPS}

.PHONY: all clean fclean re test bonus