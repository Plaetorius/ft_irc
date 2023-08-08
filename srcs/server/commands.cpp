#include "IRC.hpp"
#include "User.hpp"

vector<string>	get_command(int id_user, t_data &data)
{
	char				buff[READ_SIZE + 1];
	int					bytes_read;
	string				res;
	vector<string>		test_vect;	

	bzero(buff, READ_SIZE + 1);
	bytes_read = read(data.users[id_user]->get_fd(), buff, READ_SIZE);
	res = buff;
	test_vect.push_back(res);
	return test_vect;
}