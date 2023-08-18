#include "IRC.hpp"
#include "User.hpp"


string	read_raw_input(int	user_fd, t_data &data)
{
	char	buffer[READ_SIZE + 1];
	int		len_read;

	bzero(buffer, READ_SIZE + 1);
	len_read = read(user_fd, buffer, READ_SIZE);
	if (len_read == -1)
		clear_data_exit(data, "read() error", 1);
	return string(buffer, len_read);
}

t_command	parse_raw_input(string raw_input)
{
	t_command	result;
	string		parameter;
	size_t		ind;
	cout << "=======================================================" << endl
	<< "Raw Input Start " << endl
	<< "=======================================================" << endl
	<< raw_input << endl
	<< "=======================================================" << endl
	<< "Raw Input End" << endl
	<< "=======================================================" << endl;

	if (raw_input.empty())
		return result;
	if (raw_input[0] == ':' && raw_input.size() > 1)
	{
		ind = raw_input.find_first_of(" \r\n");
		result.prefix = raw_input.substr(1, ind);
		raw_input = raw_input.substr(result.prefix.size() + 1, raw_input.size() - result.prefix.size());
		if (raw_input[ind - result.prefix.size()] == '\n')
			return result;
		result.prefix = trim_spaces(result.prefix);
		if (raw_input.size() == 1 && string(" \r\n", 3).find(raw_input[0]))
			raw_input.clear();		
	}
	if (raw_input.empty() == false)
	{
		ind = raw_input.find_first_of(" \r\n");
		result.command = raw_input.substr(0, ind);
		raw_input = raw_input.substr(result.command.size(), raw_input.size() - result.command.size());
		if (raw_input[ind - result.command.size()] == '\n')
			return result;
		raw_input = trim_spaces(raw_input);
		if (raw_input.size() == 1 && string(" \r\n", 3).find(raw_input[0]))
			raw_input.clear();
	}
	if (raw_input.empty() == false)
	{
		while (raw_input.empty() == false && !(raw_input.size() > 1 && raw_input[0] == ':'))
		{
			ind = raw_input.find_first_of(" \r\n");
			parameter = raw_input.substr(0, ind);
			result.parameters.push_back(parameter);
			raw_input = raw_input.substr(parameter.size(), raw_input.size() - parameter.size());
			if (raw_input[ind - result.command.size()] == '\n')
				return result;
			raw_input = trim_spaces(raw_input);
			if (raw_input.size() == 1 && string(" \r\n", 3).find(raw_input[0]))
				raw_input.clear();
		}
		if (raw_input.size() > 1 && raw_input[0] == ':')
		{
			result.last_param = raw_input.substr(1);
			result.has_last_param = true;
		}
		else
			result.has_last_param = false;
	}
	return result;
}
