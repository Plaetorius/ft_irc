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
	string				channel;
	string				server_reply;
	string				result;
}						t_bot;

#endif