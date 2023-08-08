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

class User;

using namespace std;
# define MAX_CONNECTIONS 16
# define LOCAL_HOST "127.0.0.1"
# define READ_SIZE	512 //max message size. 510 "usable characters". 2 last ones MUST be \r\n

typedef std::map<int, User*> t_users;

typedef struct s_data
{
		int					socket_fd;
		struct sockaddr_in	socket_addr;
		int					epoll_fd;
		epoll_event			socket_event;
		epoll_event			events[MAX_CONNECTIONS];
		int					port;
		std::string			password;
		t_users				users;
		vector<int>			open_fds;
} 							t_data;


/******************************************************************************/
/*																		      */
/*                                  Parsing                                   */
/*																		      */
/******************************************************************************/
bool	parsing(char **argv, int &port, string &password);

/******************************************************************************/
/*																		      */
/*									Error									  */
/*																		      */
/******************************************************************************/
bool	error_str(const string error_message);

/******************************************************************************/
/*																		      */
/*									Init									  */
/*																		      */
/******************************************************************************/
bool	init(int &port, t_data &data);

/******************************************************************************/
/*																		      */
/*                                  Server           	                      */
/*																		      */
/******************************************************************************/
void	server_actions(t_data &data, int i);
vector<string>	get_command(int id_user, t_data &data);

/******************************************************************************/
/*																		      */
/*                                  Cleaning                                  */
/*																		      */
/******************************************************************************/
void	clear_data(t_data &data);
void	clear_data_exit(t_data &data, std::string err_message, int err_code);

/******************************************************************************/
/*																		      */
/*                                  Utils		                              */
/*																		      */
/******************************************************************************/
int	find_id(int fd, t_data &data);


#endif