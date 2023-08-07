#include "IRC.hpp"

void	server_actions(t_data &data, int i)
{
	int user_id = find_id(data.events[i].data.fd, data);
	(void)user_id;
	(void)data;
	(void)i;
} 
