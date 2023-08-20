#ifndef BOT_HPP
# define BOT_HPP
# include <string>

using namespace::std;

typedef struct	s_bot
{
	int					fd_socket;
	struct sockaddr_in	addr_socket;
	int					port;
	string 				password;
}						t_bot;

#endif