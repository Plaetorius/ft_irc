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
	fd_new_con = accept(data.socket_fd, (sockaddr *)&socket_new_con, &size_socket_new_con);
	if (fd_new_con < 0)
		clear_data_exit(data, "accept() failed", 1);
	new_user = new User(user_id, fd_new_con);
	data.users.insert(make_pair<int, User *>(user_id, new_user));
	data.open_fds.push_back(fd_new_con);
	epoll_event_new_con.events = EPOLLIN | EPOLLRDHUP;
	epoll_event_new_con.data.fd = fd_new_con;
	fcntl(fd_new_con, F_SETFL, O_NONBLOCK); //Imposed by the subject
	if (epoll_ctl(data.epoll_fd, EPOLL_CTL_ADD, fd_new_con, &epoll_event_new_con) < 0)
		clear_data_exit(data, "epoll_ctl() failed", 1);
	cout << "User " << user_id++ << " connected" << endl;
}

static void	user_disconnection(t_data &data, int fd)
{
	int						id_disc_user;
	User					*disc_user;
	vector<int>::iterator	it;
	vector<int>::iterator	ite;

	epoll_ctl(data.epoll_fd, EPOLL_CTL_DEL, fd, &data.socket_event);
	close(fd);
	id_disc_user = find_id(fd, data);
	try
	{
		disc_user = data.users.at(id_disc_user);
	}
	catch (std::out_of_range)
	{
		cerr << "Couldn't disconnect User with fd " << fd << "; there is no such user" << endl;
		return ;
	}
	data.users.erase(id_disc_user);
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
	cout << "User " << id_disc_user << " disconnected." << endl;
}


string	read_connection_data(int fd_user) {
	char				buff[READ_SIZE + 1];
	int					bytes_read;
	string				res;	

	
	bzero(buff, READ_SIZE + 1);
	bytes_read = read(fd_user, buff, READ_SIZE);
	return res;
}

static void	user_command(int id_user, t_data &data)
{
	static map<int, string>		command;
	
	command[id_user].append(read_connection_data(data.users[id_user]->get_fd()));

	return ;
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
	int user_id = find_id(data.events[i].data.fd, data);
	cout << "User ID " << user_id << endl;
	cout << "data.events[i].data.fd " << data.events[i].data.fd << " data.socket_fd " << data.socket_fd << endl;

	if (data.events[i].data.fd == data.socket_fd)			//Check if the user 
		user_connection(data);								//Connect a user
	if (data.events[i].events & EPOLLRDHUP)					//Check if the user has shutdown the connection
		user_disconnection(data, data.events[i].data.fd);	//Disconnect a user
	else if (user_id != -1) 								//Issue a command if the user hasn't been disconnected
		user_command(user_id, data);
} 
