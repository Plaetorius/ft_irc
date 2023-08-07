#include "User.hpp"

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


/*
				GETTERS
*/
int	User::get_fd(void) {return this->_fd;};
int User::get_id(void) {return this->_id;};


//TODO remove ; for debug
void User::set_id(int id) {this->_id = id;};
void User::set_fd(int fd) {this->_fd = fd;};