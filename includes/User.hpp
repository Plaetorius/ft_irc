#ifndef USER_H
# define USER_H
# include <string>
# include <iostream>

using namespace std;

class User
{
	private:
		int	_id;
		int	_fd;
	public:
		User();
		User(const  User &obj);
		User& operator=(const User &rhs);
		~User();

		User(int fd, int id);

		int	get_id();
		int	get_fd();

		//TODO remove ; for debug
		void set_id(int id);
		void set_fd(int fd);
};

#endif