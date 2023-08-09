#include "IRC.hpp"
#include "User.hpp"

static void	user_connection(t_data &data)
{
	int					fd_new_con;
	struct	sockaddr_in socket_new_con;
	struct	epoll_event epoll_event_new_con;
	socklen_t			size_socket_new_con;
	User				*new_user;
	static int			user_id = 1;

	size_socket_new_con = sizeof(socket_new_con);
	socket_new_con = sockaddr_in();
	epoll_event_new_con = epoll_event();
	fd_new_con = accept(data.socket.fd, (struct sockaddr *)&socket_new_con, &size_socket_new_con);
	if (fd_new_con < 0)
		clear_data_exit(data, "accept() failed", 1);
	new_user = new User(fd_new_con, user_id);
	data.users.insert(make_pair<int, User *>(fd_new_con, new_user));
	data.open_fds.push_back(fd_new_con);
	epoll_event_new_con.events = EPOLLIN | EPOLLRDHUP;
	epoll_event_new_con.data.fd = fd_new_con;
	fcntl(fd_new_con, F_SETFL, O_NONBLOCK); //Imposed by the subject
	if (epoll_ctl(data.epoll.fd, EPOLL_CTL_ADD, fd_new_con, &epoll_event_new_con) < 0)
		clear_data_exit(data, "epoll_ctl() failed", 1);
}

static void	user_disconnection(t_data &data, int fd)
{
	int						id_disc_user;
	User					*disc_user;
	vector<int>::iterator	it;
	vector<int>::iterator	ite;

	epoll_ctl(data.epoll.fd, EPOLL_CTL_DEL, fd, &data.socket.event);
	close(fd);
	try
	{
		disc_user = data.users.at(fd);
	}
	catch (out_of_range)
	{
		cerr << "Couldn't disconnect User with fd " << fd << "; there is no such user" << endl;
		return ;
	}
	id_disc_user = disc_user->get_id();
	data.users.erase(fd);
	delete disc_user;
	it = data.open_fds.begin();
	ite = data.open_fds.end();
	for (; it != ite; it++)
	{
		if (*it == fd)
		{
			data.open_fds.erase(it);
			break;
		}
	}
	cout << "User " << id_disc_user << " disconnected :(" << endl;
}

static void	user_command(int user_fd, t_data &data)
{
	vector<string> command = format_user_input(user_fd, data);

	if (command.empty())
		return ;
	vector<string>::iterator it = command.begin();
	vector<string>::iterator ite = command.end();

	for (; it != ite; it++)
	{
		cout << *it;
	}
	cout << endl;
}

/*
	Server Actions

	Perform connection, deconnection and execution of commands issued by users
	
	Input:
		t_data	&data: reference to the structure containing all the variables
		int		i: index of the "for" loop in the "main.cpp" file. Represents one fd but ISN'T one
	
*/
void	server_actions(t_data &data, int i)
{
	const int user_fd = find_user_fd(data.epoll.events[i].data.fd, data);

	if (data.epoll.events[i].data.fd == data.socket.fd)			//Check if the user 
		user_connection(data);									//Connect a user
	if ((data.epoll.events[i].events & EPOLLERR)
		|| (data.epoll.events[i].events & EPOLLHUP)
		|| (data.epoll.events[i].events & EPOLLRDHUP)
		|| !(data.epoll.events[i].events & EPOLLIN))			//Check if the user has shutdown the connection
		user_disconnection(data, data.epoll.events[i].data.fd);	//Disconnect a user
	else if (user_fd != -1 && data.users[user_fd]->get_id() != -1) 									//Issue a command if the user hasn't been disconnected
		user_command(user_fd, data);
} 
