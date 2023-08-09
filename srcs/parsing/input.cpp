#include "IRC.hpp"
#include "User.hpp"


static string	read_user_input(int	user_fd, t_data &data)
{
	char	buffer[READ_SIZE + 1];
	int		len_read;

	bzero(buffer, READ_SIZE + 1);
	len_read = read(user_fd, buffer, READ_SIZE);
	return string(buffer, len_read);
}

vector<string>	format_user_input(int user_fd, t_data &data)
{
	vector<string> res;

	res.push_back(read_user_input(user_fd, data));
	return res;
}
