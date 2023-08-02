#include "IRC.hpp"

int main(int argc, char *argv[])
{
	int			port; 
	string	password;

	if (argc != 3)
	{
		error_str("ircserver requires 2 arguments. Usage: ./ircserver <PORT> <PASSWORD>");
		return (EXIT_FAILURE);
	}
	if (!parsing(argv, port, password))
		return (EXIT_FAILURE);
	cout << "Port: " << port << endl << "Password: " << password << endl << "Enjoy ;)" << endl;
	if (!init(port, password))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}