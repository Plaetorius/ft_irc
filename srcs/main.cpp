#include "IRC.hpp"
#include "User.hpp"

/*
	TODO
	Init fds of data to -1, to check in clear_data if the fd has to be closed or not
*/

t_data	*g_data_ptr;

void	signal_handler(int code)
{
	(void)code;
	clear_data_exit(*g_data_ptr, "SIGINT", 0);
}

int main(int argc, char *argv[])
{
	t_data				data;
	int					epoll_fds;
	struct sigaction	sa;

	if (argc != 3)
		return (error_str("ircserver requires 2 arguments. Usage: ./ircserver <PORT> <PASSWORD>"), EXIT_FAILURE);
	if (!parsing(argv, data.port, data.password))
		return (EXIT_FAILURE);
	if (!init(data.port, data))
		return (EXIT_FAILURE);
	g_data_ptr = &data;
	sa.sa_handler = signal_handler;
	sa.sa_flags = SA_RESTART; 					//Set to 0 if causes problems with syscalls
	sigfillset(&sa.sa_mask);					//Blocks the other signals when the handler is called
	if (sigaction(SIGINT, &sa, NULL) < 0)
		clear_data_exit(data, "sigaction() error", 1);
	cout << "Port: " << data.port << endl << "Password: " << data.password << endl << "Enjoy ;)" << endl;
	while (true)
	{
		epoll_fds = epoll_wait(data.epoll.fd, data.epoll.events, MAX_CONNECTIONS, -1);
		if (epoll_fds < 0)
			return (clear_data(data), error_str("epoll_wait() failed"), EXIT_FAILURE);
		for (int i = 0; i < epoll_fds; i++)
			server_actions(data, i);		
	}
	return (EXIT_SUCCESS);
}
