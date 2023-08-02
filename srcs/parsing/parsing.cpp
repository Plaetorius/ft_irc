#include "IRC.hpp"

static bool parse_port(char *port_str, int &port)
{
	char *end_ptr = NULL;

	errno = 0;
	port = strtod(port_str, &end_ptr);
	if (errno == ERANGE)
	{
		error_str("port is out of range");
		return false;
	}
	if (*end_ptr != '\0')
	{
		cerr << "invalid char in \"port\" argument: " << *end_ptr << endl;
		return false;
	}
	if (port < 0)
	{
		error_str("port number can't be negative");
		return false;
	}
	if (port <= 1023)
	{
		error_str("port is system reserved port range (0-1023)");
		return false;
	}
	if (port > 65535)
	{
		error_str("port number is too big");
		return false;
	}
	return true;	
}

static bool	parse_password(char *password_str, string &password)
{
	password = password_str;
	if (password.empty() || password.size() < 1)
	{
		error_str("password can't be empty");
		return false;
	}
	return true;
}

bool parsing(char **argv, int &port, string &password)
{
	return (parse_port(argv[1], port) && parse_password(argv[2], password));
}