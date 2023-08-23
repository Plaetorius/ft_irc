#include "IRC.hpp"
#include "User.hpp"
#include "messages.hpp"

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
	new_user = new User(user_id, fd_new_con);
	data.users.insert(make_pair<int, User *>(fd_new_con, new_user));
	data.open_fds.push_back(fd_new_con);
	epoll_event_new_con.events = EPOLLIN | EPOLLRDHUP;
	epoll_event_new_con.data.fd = fd_new_con;
	fcntl(fd_new_con, F_SETFL, O_NONBLOCK); //Imposed by the subject
	if (epoll_ctl(data.epoll.fd, EPOLL_CTL_ADD, fd_new_con, &epoll_event_new_con) < 0)
		clear_data_exit(data, "epoll_ctl() failed", 1);
	cout << "\033[1;" << 30 + user_id % 7 << "m" << "User " << user_id << " connected :)" << endl;
	++user_id;
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
	cout << "\033[1;" << 30 + id_disc_user % 7 << "m" << "User " << id_disc_user << " disconnected :(" << endl;
}

/*
	execute_commands() will execute all the commands in the list of the user
*/
void	execute_commands(t_command &command, User *user)
{
	while (user->_commands.empty() == false)
	{
		command = user->_commands.front();
		user->_commands.pop_front(); 
		bool    result;
		int     quit_fd;

		if (command.command == "PASS") {
			if (user->command_PASS(command) == false)
				user_disconnection(*g_data_ptr, user->get_fd());
		} else if (command.command == "NICK") {
			result = user->command_NICK(command);
		} else if (command.command == "USER") {
			result = user->command_USER(command);
		} else if (command.command == "PING") {
			result = user->command_PING(command);
		} else if (command.command == "JOIN") {
			result = user->command_JOIN(command);
		} else if (command.command == "NAMES") {
			result = user->command_names(command);
		} else if (command.command == "PRIVMSG") {
			result = user->command_PRIVMSG(command);
		} else if (command.command == "QUIT") {
			quit_fd = user->command_QUIT(command);
			user_disconnection(*g_data_ptr, quit_fd);
			break;
		} else if (command.command == "KILL" || command.command == "kill") {
			quit_fd = user->command_KILL(command);
			if (quit_fd != -1)
				user_disconnection(*g_data_ptr, quit_fd);
		} else if (command.command == "PART") {
			result = user->command_PART(command);
		} else if (command.command == "KICK") {
			result = user->command_KICK(command);
		} else if (command.command == "OPER") {
			result = user->command_OPER(command);
		} else if (command.command == "MODE") {
			result = user->command_MODE(command);
		} else if (command.command == "TOPIC") {
			result = user->command_TOPIC(command);
		} else if (command.command == "NOTICE") {
			result = user->command_NOTICE(command);
		} else if (command.command == "INVITE") {
			result = user->command_INVITE(command);
		} else {
			user->command_unknown(command);
		}
	}
}

static void	user_command(int user_fd, t_data &data)
{
	string	raw_input = read_raw_input(user_fd);
	string	current;
	t_command command;
	t_users::iterator it, ite;
	vector<string>::iterator it_param, ite_param;
	string	tmp;
	
	while (raw_input.empty() == false)
	{
		current = raw_input.substr(0, raw_input.find_first_of("\n"));
		command = parse_raw_input(current);
		if (raw_input.size() >= current.size() + 1)
			raw_input = raw_input.substr(current.size() + 1, raw_input.size());
		else
			raw_input.clear();
		data.users[user_fd]->push_back_command(command);
	}
	if (command.command.size() == 0) {
        data.users[user_fd]->send_message("Where is my command, frere?!\r\n");
        return ;
    }
	execute_commands(command, data.users[user_fd]);
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

	if (data.epoll.events[i].data.fd == data.socket.fd)				//Check if the user 
		user_connection(data);										//Connect a user
	if ((data.epoll.events[i].events & EPOLLERR)
		|| (data.epoll.events[i].events & EPOLLHUP)
		|| (data.epoll.events[i].events & EPOLLRDHUP)
		|| !(data.epoll.events[i].events & EPOLLIN))				//Check if the user has shutdown the connection
		user_disconnection(data, data.epoll.events[i].data.fd);		//Disconnect a user
	else if (user_fd != -1 && data.users[user_fd]->get_id() != -1) 	//Issue a command if the user hasn't been disconnected
		user_command(user_fd, data);
} 
