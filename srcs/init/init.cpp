#include "IRC.hpp"

bool	init(int &port, t_data &data)
{
	int	sock_opt_val = 1;

	data.socket.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (data.socket.fd < 0)
		return error_str("socket() error");
	data.socket.addr = sockaddr_in();
	data.socket.addr.sin_family = AF_INET;
	data.socket.addr.sin_port = htons(port);
	data.socket.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt(data.socket.fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt_val, sizeof(sock_opt_val)) < 0)
		return close(data.socket.fd), error_str("setsockopt() error");
	if (bind(data.socket.fd, (struct sockaddr *)(&data.socket.addr), sizeof(data.socket.addr)) < 0)
		return close(data.socket.fd), error_str("Port already in use. Use \"ps -aux\" to find an unused port");
	if (listen(data.socket.fd, MAX_CONNECTIONS) < 0)
		return close(data.socket.fd), error_str("listen() error");
	data.epoll.fd = epoll_create1(EPOLL_CLOEXEC);
	if (data.epoll.fd < 0)
		return close(data.socket.fd), error_str("epoll_create() error");
	data.socket.event.data.fd = data.socket.fd;
	data.socket.event.events = EPOLLIN;
	if (epoll_ctl(data.epoll.fd, EPOLL_CTL_ADD, data.socket.fd, &data.socket.event) < 0)
		return close(data.epoll.fd), close(data.socket.fd), error_str("epoll_ctl() fail");
	return true;
}
