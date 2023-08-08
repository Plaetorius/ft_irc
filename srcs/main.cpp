#include "IRC.hpp"
#include "User.hpp"

/*
	TODO
	Init fds of data to -1, to check in clear_data if the fd has to be closed or not
*/

int main(int argc, char *argv[])
{
	int			port; 
	std::string	password;
	t_data		data;
	int			epoll_fds;

	if (argc != 3)
		return (error_str("ircserver requires 2 arguments. Usage: ./ircserver <PORT> <PASSWORD>"), EXIT_FAILURE);
	if (!parsing(argv, port, password))
		return (EXIT_FAILURE);
	if (!init(port, data))
		return (EXIT_FAILURE);
	cout << "Port: " << port << endl << "Password: " << password << endl << "Enjoy ;)" << endl;
	while (true)
	{
		epoll_fds = epoll_wait(data.epoll_fd, data.events, MAX_CONNECTIONS, -1);
		if (epoll_fds < 0)
			return (clear_data(data), error_str("epoll_wait() failed"), EXIT_FAILURE);
		for (int i = 0; i < epoll_fds; i++)
			server_actions(data, i);		
	}
	return (EXIT_SUCCESS);
}