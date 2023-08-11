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
/*                               	START UTILS                               */
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
/*                               	END UTILS                                 */
/*																		      */
/******************************************************************************/

/******************************************************************************/
/*																		      */
/*                              START FUNCTIONS                               */
/*																		      */
/******************************************************************************/

bool	Channel::add_user(int fd_user, t_data &data)
{
	if (fd_user < 5)
	{
		return error_str("Invalid user tries to join the channel " + this->_name);
	}
	this->_fds_users.push_back(fd_user);
	this->broadcast("User " + int_to_string(fd_user) + " joined the channel!", fd_user);
	return true;
}

bool	Channel::kick_user(int fd_emitter, int fd_to_kick, string message, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator");
	if (fd_emitter == fd_to_kick)
		return error_feedback(fd_emitter, "You can't kick yourself");
	if (is_user(fd_to_kick) == false)
		return error_feedback(fd_emitter, "There is no user " + int_to_string(fd_to_kick));
	this->_fds_users.erase(find(this->_fds_users.begin(), this->_fds_users.end(), fd_to_kick));
	message = trim_spaces(message);
	if (message.empty())
		message = "You have been kicked by an operator";
	this->broadcast("User " + int_to_string(fd_to_kick) + " was kicked from " + this->_name + " by User " + int_to_string(fd_emitter) + " for: " + message + "\n", fd_emitter);
	return true;
}

bool	Channel::op_user(int fd_emitter, int fd_to_op, t_data &data)
{
	string message;

	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator");
	if (fd_to_op < 5)
		return error_feedback(fd_emitter, "Can't OP: this user doesn't exist");
	if (is_user(fd_to_op) == false)
		return error_feedback(fd_emitter, "Can't OP: this user isn't in the channel");
	this->_fds_ops.push_back(fd_to_op);
	this->broadcast(data.users.at(fd_to_op)->get_nick() + " is now channel operator!", fd_emitter);
	return true;
}

void	Channel::set_topic(string topic, int fd_emitter, t_data &data)
{
	if (topic.empty())
		return ;
	this->_topic = topic;
	this->broadcast("User " + int_to_string(fd_emitter) + " has changed topic to " + topic, fd_emitter);
}

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

const string	Channel::get_topic(void) const
{
	return this->_topic;
}
