#include "IRC.hpp"
#include "User.hpp"


static string	read_user_input(int	user_fd, t_data &data)
{
	char	buffer[READ_SIZE + 1];
	int		len_read;

	bzero(buffer, READ_SIZE + 1);
	len_read = read(user_fd, buffer, READ_SIZE);
	if (len_read == -1)
		clear_data_exit(data, "read() error", 1);
	return string(buffer, len_read);
}

static t_command	parse_raw_input(string &raw_input)
{
	t_command	result;
	string		parameter;

	if (raw_input.empty())
		return result;
	if (raw_input[0] == ':' && raw_input.size() > 1)
	{
		result.prefix = raw_input.substr(1, raw_input.find_first_of(" \r\n") - 1);
		raw_input = raw_input.substr(result.prefix.size() + 2, raw_input.size() - result.prefix.size() - 3);
	}
	if (raw_input.empty() == false)
	{
		result.command = raw_input.substr(0, raw_input.find_first_of(" \r\n"));
		raw_input = raw_input.substr(result.command.size(), raw_input.size() - result.command.size());
		raw_input = trim_spaces(raw_input);
		if (raw_input.size() == 1 && string(" \r\n", 3).find(raw_input[0]))
			raw_input.clear();
	}
	if (raw_input.empty() == false)
	{
		while (raw_input.empty() == false && !(raw_input.size() > 1 && raw_input[0] == ':'))
		{
			parameter = raw_input.substr(0, raw_input.find_first_of(" \r\n"));
			result.parameters.push_back(parameter);
			raw_input = raw_input.substr(parameter.size(), raw_input.size() - parameter.size());
			raw_input = trim_spaces(raw_input);
			if (raw_input.size() == 1 && string(" \r\n", 3).find(raw_input[0]))
				raw_input.clear();
		}
		if (raw_input.size() > 1 && raw_input[0] == ':')
			result.last_param = raw_input.substr(1);
	}
	return result;
}

t_command	format_user_input(int user_fd, t_data &data)
{
	string	raw_input = read_user_input(user_fd, data);

	return parse_raw_input(raw_input);
}
