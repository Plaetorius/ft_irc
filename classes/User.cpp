#include "User.hpp"

User::User()
{
	cout << "Default User constructor called" << endl;
}

User::User(const User &obj)
{
	cout << "Copy User constructor called" << endl; //TODO
	(void)obj;
}

User& User::operator=(const User &rhs)
{
	cout << "User Assignment operator called" << endl; //TODO
	if (this == &rhs)
		return (*this);
	(void)rhs;
	return (*this);
}

User::~User() {}

User::User(int fd, int id) : _id(id), _fd(fd) {}

void	User::push_back_command(t_command &command)
{
	this->_commands.push_back(command);
}

/*
	execute_commands() will execute all the commands in the list of the user
*/
void	User::execute_commands()
{
	cout << "============================================" << endl << "Start execute commands" << endl <<  "============================================" << endl;
	while (this->_commands.empty() == false)
	{
		cout << this->_commands.front().command << endl;
		this->_commands.pop_front(); 
	}
	cout << "============================================" << endl << "End execute commands" << endl <<  "============================================" << endl;
}

/*
				GETTERS
*/
int	User::get_fd(void) const {return this->_fd;};
int User::get_id(void) const {return this->_id;};
string User::get_nick(void) const {return this->_nick;};

//TODO remove ; for debug
void User::set_id(int id) {this->_id = id;};
void User::set_fd(int fd) {this->_fd = fd;};