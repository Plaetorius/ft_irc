#include "IRC.hpp"
#include "User.hpp"

int	nick_to_fd(string &nick, t_data &data)
{
	map<int, User*>::iterator it;
	map<int, User*>::iterator ite;

	it = data.users.begin();
	ite = data.users.end();
	for (; it != ite; it++)
		if (nick == it->second->get_nick())
			return it->second->get_fd();
	return -1;
}