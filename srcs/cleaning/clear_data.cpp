#include "IRC.hpp"

void	clear_data(t_data &data)
{
	if (data.socket_fd > -1)
	{
		close(data.socket_fd);
		data.socket_fd = -1;
	}
	if (data.epoll_fd > -1)
	{
		close(data.epoll_fd);
		data.epoll_fd = -1;
	}
}

void	clear_data_exit(t_data &data, std::string err_message, int err_code)
{
	clear_data(data);
	error_str(err_message);
	exit(err_code);
}