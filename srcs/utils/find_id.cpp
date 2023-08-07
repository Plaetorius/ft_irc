#include "IRC.hpp"
#include "User.hpp"

int	find_id(int fd, t_data &data)
{
	t_users::const_iterator	it = data.users.begin();
	t_users::const_iterator	ite = data.users.end();
	for (; it != ite; it++)
		if (it->second->get_fd() == fd)
			return (it->second->get_id());
	return -1;
}