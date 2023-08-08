#include "IRC.hpp"

bool	init(int &port, t_data &data)
{
	int	sock_opt_val = 1;
	data.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (data.socket_fd < 0)
		return error_str("socket() error");
	data.socket_addr = sockaddr_in();
	data.socket_addr.sin_family = AF_INET;
	data.socket_addr.sin_port = htons(port);
	data.socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt(data.socket_fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt_val, sizeof(sock_opt_val)) < 0)
		return close(data.socket_fd), error_str("setsockopt() error");
	if (bind(data.socket_fd, (struct sockaddr *)(&data.socket_addr), sizeof(data.socket_addr)) < 0)
		return close(data.socket_fd), error_str("Port already in use. Use \"ps -aux\" to find an unused port");
	if (listen(data.socket_fd, MAX_CONNECTIONS) < 0)
		return close(data.socket_fd), error_str("listen() error");
	data.epoll_fd = epoll_create1(0);
	if (data.epoll_fd < 0)
		return close(data.socket_fd), error_str("epoll_create() error");
	data.socket_event.data.fd = data.socket_fd;
	data.socket_event.events = EPOLLIN;
	if (epoll_ctl(data.epoll_fd, EPOLL_CTL_ADD, data.socket_fd, &data.socket_event) < 0)
		return close(data.epoll_fd), close(data.socket_fd), error_str("epoll_ctl() fail");
	return true;
}
