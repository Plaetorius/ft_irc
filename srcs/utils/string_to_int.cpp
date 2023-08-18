#include "IRC.hpp"

int	string_to_int(string &number)
{
	int value;

	istringstream iss(number);
	iss >> value;
	return value;
}