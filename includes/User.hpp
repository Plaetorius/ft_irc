#ifndef USER_H
# define USER_H
# include <string>
# include <iostream>
# include "IRC.hpp"

using namespace std;

class User
{
	private:
		int	_id;
		int	_fd;
		list<t_command> _commands;
	public:
		User();
		User(const  User &obj);
		User& operator=(const User &rhs);
		~User();

		User(int fd, int id);

		int	get_id();
		int	get_fd();

		void	push_back_command(t_command &command); //TODO add in the input while loop
		void	execute_commands();

		//TODO remove ; for debug
		void set_id(int id);
		void set_fd(int fd);
};

#endif