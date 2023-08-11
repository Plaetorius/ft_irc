#ifndef IRC_HPP
# define IRC_HPP
# include <iostream>
# include <string>
# include <map>
# include <list>
# include <vector>
# include <algorithm>
# include <errno.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <signal.h>
# include <sstream>

class User;

using namespace std;
# define MAX_CONNECTIONS 16
# define LOCAL_HOST "127.0.0.1"
# define READ_SIZE	512 			//Max message size. 510 "usable characters". 2 last ones MUST be \r\n
# define DEBUG false

typedef map<int, User*> t_users; 	//Users will be mapped key: fd; User*: User corresponding to the fd

typedef struct s_epoll
{
	int					fd;
	epoll_event			events[MAX_CONNECTIONS];
}						t_epoll;

typedef struct s_socket
{
	int					fd;
	epoll_event			event;
	struct sockaddr_in	addr;
}						t_socket;

typedef struct s_command
{
	string				prefix;
	string				command;
	vector<string>		parameters;
	string				last_param;
}						t_command;

typedef struct s_data
{
		t_epoll			epoll;
		t_socket		socket;
		int				port;
		string			password;
		t_users			users;
		vector<int>		open_fds;
} 						t_data;

extern t_data	*g_data_ptr;

/******************************************************************************/
/*																		      */
/*                                  Parsing                                   */
/*																		      */
/******************************************************************************/
bool		parsing(char **argv, int &port, string &password);
string		read_raw_input(int	user_fd, t_data &data);
t_command	parse_raw_input(string raw_input);

/******************************************************************************/
/*																		      */
/*									Error									  */
/*																		      */
/******************************************************************************/
bool		error_str(const string error_message);

/******************************************************************************/
/*																		      */
/*									Init									  */
/*																		      */
/******************************************************************************/
bool		init(int &port, t_data &data);

/******************************************************************************/
/*																		      */
/*                                  Server           	                      */
/*																		      */
/******************************************************************************/
void		server_actions(t_data &data, int i);

/******************************************************************************/
/*																		      */
/*                                  Cleaning                                  */
/*																		      */
/******************************************************************************/
void		clear_data(t_data &data);
void		clear_data_exit(t_data &data, string message, int err_code);
;
/******************************************************************************/
/*																		      */
/*                                 Utils		                              */
/*																		      */
/******************************************************************************/
int			find_user_fd(int fd, t_data &data);
string		int_to_string(int n);
string		trim_spaces(const string &to_trim);
int			nick_to_fd(string &nick, t_data &data);

#endif