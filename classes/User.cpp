#include "User.hpp"
#include <iostream>

User::User()
{
	cout << "Default User constructor called" << endl;
}

User::User(const User &obj)
{
	cout << "Copy User constructor called" << endl;
	(void)obj;
}

User& User::operator=(const User &rhs)
{
	cout << "User Assignment operator called" << endl;
	if (this == &rhs)
		return (*this);
	(void)rhs;
	return (*this);
}

User::~User()
{
	cout << "User destructor called" << endl;
}