#include "IRC.hpp"

bool	init(int &port, t_data &data)
{
	data.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (data.socket_fd < 0)
		return error_str("couldn't open socket");
	memset(&data.serv_addr, '0', sizeof(data.serv_addr)); //TODO maybe consider a cpp alternative
	data.serv_addr.sin_family = AF_INET;
	data.serv_addr.sin_port = htons(port);
	data.serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(data.socket_fd, (struct sockaddr *)(&data.serv_addr), sizeof(data.serv_addr)) < 0)
		return close(data.socket_fd), error_str("couldn't bind socket. Use \"ps -aux\" to find an unused port");
	if (listen(data.socket_fd, MAX_CONNECTIONS) < 0)
		return close(data.socket_fd), error_str("listen() function error");
	data.epoll_fd = epoll_create1(0);
	if (data.epoll_fd < 0)
		return close(data.socket_fd), error_str("epoll create fail");
	data.socket_event.data.fd = data.socket_fd;
	data.socket_event.events = EPOLLIN;
	if (epoll_ctl(data.epoll_fd, EPOLL_CTL_ADD, data.socket_event.data.fd, &data.socket_event) < 0)
		return close(data.epoll_fd), close(data.socket_fd), error_str("epoll ctl fail");
	return true;
}
