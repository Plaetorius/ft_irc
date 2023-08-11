#include "IRC.hpp"
#include "User.hpp"

int	find_user_fd(int fd, t_data &data)
{
	t_users::const_iterator	it;
	t_users::const_iterator	ite;

	it = data.users.begin();
	ite = data.users.end();
	for (; it != ite; it++)
		if (it->first == fd)
			return (it->first);
	return -1;
}