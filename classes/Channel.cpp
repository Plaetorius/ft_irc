#include "Channel.hpp"
#include <iostream>
#include "User.hpp"

/******************************************************************************/
/*																		      */
/*                                START RO3                                   */
/*																		      */
/******************************************************************************/

Channel::Channel()
{
	std::cout << "Default Channel constructor called" << std::endl;
}

Channel::Channel(const std::string name): _name(name)
{
	std::cout << "Channel Name constructor called" << std::endl;
}

Channel::Channel(const Channel &obj)
{
	(void)obj;
	std::cout << "Copy Channel constructor called" << std::endl;
}

Channel& Channel::operator=(const Channel &rhs)
{
	(void)rhs;
	std::cout << "Channel Assignment operator called" << std::endl;
	if (this == &rhs)
		return (*this);
	return (*this);
}

Channel::~Channel()
{
	std::cout << "Channel destructor called" << std::endl;
}

/******************************************************************************/
/*																		      */
/*                                  END RO3                                   */
/*																		      */
/******************************************************************************/

/******************************************************************************/
/*																		      */
/*                               	START CHECKERS                            */
/*																		      */
/******************************************************************************/

bool	Channel::is_user(int fd_user)
{
	vector<int>::iterator it = this->_fds_users.begin();
	vector<int>::iterator ite = this->_fds_users.end();

	for (; it != ite; it++)
	{
		if (*it == fd_user)
			return true;
	}
	return false;
}

bool	Channel::is_op(int fd_user)
{
	vector<int>::iterator it = this->_fds_ops.begin();
	vector<int>::iterator ite = this->_fds_ops.end();

	for (; it != ite; it++)
	{
		if (*it == fd_user)
			return true;
	}
	return false;
}

bool	Channel::is_invited(int fd_user)
{
	vector<int>::iterator it = this->_fds_invited.begin();
	vector<int>::iterator ite = this->_fds_invited.end();

	for (; it != ite; it++)
	{
		if (*it == fd_user)
			return true;
	}
	return false;
}

bool	Channel::error_feedback(int fd_emitter, string message)
{
	message += "\n";
	write(fd_emitter, message.c_str(), message.size());
	return false;
}

/******************************************************************************/
/*																		      */
/*                               	END CHECKERS                              */
/*																		      */
/******************************************************************************/

/******************************************************************************/
/*																		      */
/*                              START FUNCTIONS                               */
/*																		      */
/******************************************************************************/

int	Channel::add_user(int fd_user, t_data &data)
{
	if (fd_user < 5)
		return 1; //"Invalid user tries to join the channel "
	if (this->get_invite_only() == true && is_invited(fd_user) == false)
		return 2; //User not invited && channel is invite-only
	if (this->_fds_users.empty())
		this->_fds_ops.push_back(fd_user);
	this->_fds_users.push_back(fd_user);
	this->broadcast(data.users.at(fd_user)->get_nick() + " joined the channel!", fd_user);
	return 0;
}

int	Channel::kick_user(int fd_emitter, int fd_to_kick, string message, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return 1; //"You are not channel operator"
	if (fd_emitter == fd_to_kick)
		return 2; //"You can't kick yourself"
	if (is_user(fd_to_kick) == false)
		return 3; // "User doesn't exist"
	this->_fds_users.erase(find(this->_fds_users.begin(), this->_fds_users.end(), fd_to_kick));
	message = trim_spaces(message);
	if (message.empty())
		message = "You have been kicked by an operator";
	this->broadcast(data.users.at(fd_to_kick)->get_nick() + " was kicked from " + this->_name + " by " + data.users.at(fd_emitter)->get_nick() + " for: " + message + "\n", fd_emitter);
	return 0;
}

int	Channel::op_user(int fd_emitter, int fd_to_op, t_data &data)
{
	string message;

	if (is_op(fd_emitter) == false)
		return 1; // "You are not channel operator"
	if (fd_to_op < 5)
		return 2; //"Can't OP: this user doesn't exist"
	if (is_user(fd_to_op) == false)
		return 3; //"Can't OP: this user isn't in the channel"
	//TODO keep or delete
	// if (is_op(fd_to_op) == false)
	// 	return error_feedback(fd_emitter, "Can't OP: this user is already channel operator");
	this->_fds_ops.push_back(fd_to_op);
	this->broadcast(data.users.at(fd_to_op)->get_nick() + " is now channel operator!", fd_emitter);
	return 0;
}

int	Channel::invite_user(int fd_emitter, int fd_to_invite, t_data &data)
{
	string message;

	if (is_op(fd_emitter) == false)
		return 1; //"You are not channel operator"
	if (fd_to_invite < 5)
		return 2; //"Can't invite: this user doesn't exist"
	//TODO keep or delete, if kept delete the next if but keep the code
	// if (is_invited(fd_to_invite))
	// 	return error_feedback(fd_emitter, "Can't invite: this user is already invited");
	if (is_invited(fd_to_invite) == false)
		this->_fds_invited.push_back(fd_to_invite);
	message = data.users.at(fd_emitter)->get_nick() + " invited you to join #" + this->get_name(); 
	write(fd_to_invite, message.c_str(), message.size());
	return 0;
}

/******************************************************************************/
/*																		      */
/*                              END FUNCTIONS                                 */
/*																		      */
/******************************************************************************/

/******************************************************************************/
/*																		      */
/*                              START SETTERS  	                              */
/*																		      */
/******************************************************************************/

int	Channel::set_invite_only(bool mode, int fd_emitter)
{
	if (is_op(fd_emitter) == false)
		return 1; //"You are not channel operator" 
	this->_is_invite_only = mode;
	if (this->_is_invite_only == true)
	{
		add_users_to_invited();
		broadcast("#" + this->get_name() + " is now invite only.", fd_emitter);
	}
	else
		broadcast("#" + this->get_name() + " is no longer invite only.", fd_emitter);
	return 0;
}

int	Channel::set_topic(string topic, int fd_emitter, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return 1; //"You are not channel operator" 
	this->_topic = topic;
	this->broadcast(data.users.at(fd_emitter)->get_nick() +  " has changed topic to " + topic, fd_emitter);
	return 0;
}

/******************************************************************************/
/*																		      */
/*                              END SETTERS	                                  */
/*																		      */
/******************************************************************************/

/******************************************************************************/
/*																		      */
/*                              START UTILS		                              */
/*																		      */
/******************************************************************************/

void	Channel::broadcast(string message, int fd_emitter)
{
	vector<int>::iterator it = this->_fds_users.begin();
	vector<int>::iterator ite = this->_fds_users.end();

	(void)fd_emitter;
	for (; it != ite; it++)
	{
		write(*it, message.c_str(), message.size());
	}
}

void	Channel::add_users_to_invited(void)
{
	vector<int>::iterator	it_users = this->_fds_users.begin();
	vector<int>::iterator	ite_users = this->_fds_users.end();
	for(; it_users != ite_users; it_users++)
		if (is_invited(*it_users) == false)
			this->_fds_invited.push_back(*it_users);
}

/******************************************************************************/
/*																		      */
/*                              END UTILS		                              */
/*																		      */
/******************************************************************************/

/******************************************************************************/
/*																		      */
/*                            START GETTERS		                              */
/*																		      */
/******************************************************************************/

string		Channel::get_name(void) const {return this->_name;};
vector<int>	Channel::get_users(void) const {return this->_fds_users;};
vector<int>	Channel::get_ops(void) const {return this->_fds_ops;};
vector<int>	Channel::get_invited(void) const {return this->_fds_invited;};
bool		Channel::get_invite_only(void) const {return this->_is_invite_only;};
string		Channel::get_topic(void) const {return this->_topic;}

/******************************************************************************/
/*																		      */
/*                            END GETTERS		                              */
/*																		      */
/******************************************************************************/

