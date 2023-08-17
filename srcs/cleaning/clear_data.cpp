#include "IRC.hpp"
#include "User.hpp"

void	clear_data(t_data &data)
{
	if (data.socket.fd > -1)
	{
		close(data.socket.fd);
		data.socket.fd = -1;
	}
	if (data.epoll.fd > -1)
	{
		close(data.epoll.fd);
		data.epoll.fd = -1;
	}
	t_users::iterator it_users = data.users.begin();
	t_users::iterator ite_users = data.users.end();
	for (; it_users != ite_users; it_users++)
		delete	it_users->second;
	vector<int>::iterator  it_open_fds = data.open_fds.begin();
	vector<int>::iterator  ite_open_fds = data.open_fds.end();
	for (; it_open_fds != ite_open_fds; it_open_fds++)
		close(*it_open_fds);
	t_channels::iterator it_channels = data.channels.begin();
	t_channels::iterator ite_channels = data.channels.end();
	for (; it_channels != ite_channels; it_channels++)
		delete it_channels->second;
	data.channels.clear();
	data.users.clear();
	data.open_fds.~vector();
}

void	clear_data_exit(t_data &data, std::string message, int err_code)
{
	clear_data(data);
	if (err_code)
		error_str(message);
	else
		cout << message << endl;
	exit(err_code);
}