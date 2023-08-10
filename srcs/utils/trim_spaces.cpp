#include "IRC.hpp"

string	trim_spaces(const string &to_trim)
{
	size_t	start_pos = to_trim.find_first_not_of(" ");
	size_t	end_pos = to_trim.find_last_not_of(" "); 


	if (string::npos != start_pos && string::npos != end_pos)
		return to_trim.substr(start_pos, end_pos - start_pos + 1);
	return "";
}