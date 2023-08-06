#ifndef USER_H
# define USER_H
# include "IRC.hpp"

class User
{
	private:

	public:
		User();
		User(const  User &obj);
		User& operator=(const User &rhs);
		~User();
};

#endif