NAME     = ircserv
CC       = c++
DEP_FLAG = -MMD
C98      = -std=c++98
FLAGS    = -Wall -Wextra -Werror ${C98} ${DEP_FLAG} -g3
INCL     = -I includes
SRCS	=	$(addsuffix .cpp,		\
			$(addprefix srcs/,		\
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
				commands_channel	\
				commands			\
			)						\
			$(addprefix cleaning/,	\
				clear_data			\
			)						\
			$(addprefix utils/,		\
				find_user_fd		\
				int_to_string		\
				string_to_int		\
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
			$(addprefix srcs/,		\
			$(addprefix parsing/,	\
				parsing				\
			)))
MAIN_OBJ = srcs/main.o
BOT_OBJ  = bot/bot.o
OBJS    = ${SRCS:.cpp=.o} ${CLAS:.cpp=.o}
BOBJS   = ${BONUS:.cpp=.o} ${CLAS:.cpp=.o}
DEPS    = ${OBJS:.o=.d}
BDEPS   = ${BOBJS:.o=.d}
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

${NAME}: ${OBJS} ${MAIN_OBJ}
	@${CC} ${FLAGS} ${OBJS} ${MAIN_OBJ} ${INCL} -o $@ 
	@echo "${LGREEN}Successfully created ${NC}${CYAN}${NAME}${NC}${LGREEN}!${NC}"

all: ${NAME}

bonus: ${OBJS} ${BOT_OBJ}
	@${CC} -g3 ${FLAGS} ${OBJS} ${BOT_OBJ} ${INCL} -o tosser
	@echo "${LGREEN}Successfully created ${NC}${CYAN}Tosser${NC}${LGREEN}!${NC}"

clean:
	@echo "${CYAN}Cleaned ${NAME}${NC}"
	@${RM} ${OBJS} ${MAIN_OBJ} ${DEPS}

fclean:
	@echo "${CYAN}FCleaned ${NAME}${NC}"
	@${RM} ${OBJS} ${NAME} ${MAIN_OBJ} ${DEPS}

re: fclean all

bonus_clean:
	@echo "${CYAN}Cleaned Tosser${NC}"
	@${RM} ${BOBJS} ${BOT_OBJ} ${DEPS}

bonus_fclean:
	@echo "${CYAN}Cleaned Tosser${NC}"
	@${RM} ${BOBJS} tosser ${BOT_OBJ} ${DEPS}

bonus_re: bonus_fclean bonus

-include ${DEPS}

.PHONY: all clean fclean re bonus