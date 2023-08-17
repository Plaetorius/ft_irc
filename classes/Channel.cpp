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
	cout << "Default Channel constructor called" << endl;
}

Channel::Channel(string name, int fd_creator, t_data &data) : _name(name), _is_invite_only(false), _is_topic_set(false), _is_topic_protected(false), _is_channel_locked(false), _max_users(0), _has_user_limit(false) 
{
	this->add_user(fd_creator, data);
	cout << "Name, Fd Creator Channel Constructor called" << endl;
}

Channel::~Channel()
{
	cout << "Channel destructor called" << endl;
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

bool	Channel::add_user(int fd_user, t_data &data)
{
	if (this->_is_invite_only == true && is_invited(fd_user) == false)
		return error_feedback(fd_user, "You must be invited to join this channel"); //User not invited && channel is invite-only
	if (this->_max_users <= this->_fds_users.size())
		return error_feedback(fd_user, "This channel is already full");
	if (this->_is_invite_only == true)
		this->_fds_invited.erase(find(this->_fds_invited.begin(), this->_fds_invited.end(), fd_user));
	if (this->_fds_users.empty())
		this->_fds_ops.push_back(fd_user);
	this->_fds_users.push_back(fd_user);
	this->broadcast(data.users.at(fd_user)->get_nick() + " joined the channel!");
	return true;
}

bool	Channel::add_user(int fd_user, string &key, t_data &data)
{
	if (this->_is_invite_only == true && is_invited(fd_user) == false)
		return error_feedback(fd_user, "You must be invited to join this channel"); //User not invited && channel is invite-only
	if (this->_max_users <= this->_fds_users.size())
		return error_feedback(fd_user, "This channel is already full");
	if (key != this->_key)
		return error_feedback(fd_user, "Wrong password");
	if (this->_is_invite_only == true)
		this->_fds_invited.erase(find(this->_fds_invited.begin(), this->_fds_invited.end(), fd_user));
	if (this->_fds_users.empty())
		this->_fds_ops.push_back(fd_user);
	this->_fds_users.push_back(fd_user);
	this->broadcast(data.users.at(fd_user)->get_nick() + " joined the channel!");
	return true;
}

bool	Channel::kick_user(int fd_emitter, int fd_to_kick, string message, t_data &data)
{
	//TODO if Channel becomes empty, delete the Channel

	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	if (is_user(fd_to_kick) == false)
		return error_feedback(fd_emitter, "Invalid user to kick");
	this->_fds_users.erase(find(this->_fds_users.begin(), this->_fds_users.end(), fd_to_kick));
	message = trim_spaces(message);
	if (message.empty())
		message = "You have been kicked by an operator";
	this->broadcast(data.users.at(fd_to_kick)->get_nick() + " was kicked from " + this->_name + " by " + data.users.at(fd_emitter)->get_nick() + " for: " + message);
	return true;
}

bool	Channel::part(int fd_user, t_data &data)
{
	//TODO if Channel becomes empty, delete the Channel
	string message;

	message = "You left #" + this->get_name() +"\n";
	write(fd_user, message.c_str(), message.size());
	this->_fds_users.erase(find(this->_fds_users.begin(), this->_fds_users.end(), fd_user));
	this->broadcast(data.users.at(fd_user)->get_nick() + " left the channel!");
	return true;
}

bool	Channel::op_user(int fd_emitter, int fd_to_op, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	if (is_user(fd_to_op) == false)
		return error_feedback(fd_emitter, "Invalid user to op");
	if (is_op(fd_to_op) == false)
		this->_fds_ops.push_back(fd_to_op);
	this->broadcast(data.users.at(fd_to_op)->get_nick() + " is now channel operator!");
	return true;
}

bool	Channel::invite_user(int fd_emitter, int fd_to_invite, t_data &data)
{
	string message;

	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	if (is_invited(fd_to_invite) == false)
		this->_fds_invited.push_back(fd_to_invite);
	message = data.users.at(fd_emitter)->get_nick() + " invited you to join #" + this->get_name() +"\n"; 
	write(fd_to_invite, message.c_str(), message.size());
	return true;
}

bool	Channel::deop_user(int fd_emitter, int fd_to_deop, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	if (is_user(fd_to_deop) == false)
		return error_feedback(fd_emitter, "Invalid user to deop");
	this->_fds_ops.erase(find(this->_fds_ops.begin(), this->_fds_ops.end(), fd_to_deop));
	this->broadcast(data.users.at(fd_to_deop)->get_nick() + " is no longer channel operator!");
	return true;
}

bool	Channel::enable_locked_mode(int fd_emitter, string &key, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	if (key.empty() == true)
		return disable_locked_mode(fd_emitter, data);
	this->_is_channel_locked = true;;
	this->_key = key;
	this->broadcast("Channel is now locked.");
	return true;
}

bool	Channel::disable_locked_mode(int fd_emitter, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	this->_is_channel_locked = false;
	this->_key = "";
	this->broadcast("Channel is no longer locked.");
	return true;
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

bool	Channel::set_invite_only(bool mode, int fd_emitter)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	this->_is_invite_only = mode;
	this->_fds_invited.clear();
	if (this->_is_invite_only == true)
		broadcast("#" + this->get_name() + " is now invite only.");
	else
		broadcast("#" + this->get_name() + " is no longer invite only");
	return true;
}

bool	Channel::set_topic(string &topic, int fd_emitter, t_data &data)
{
	if (this->_is_topic_protected && is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator"); 
	this->_is_topic_set = true;
	this->_topic = topic;
	this->broadcast(data.users.at(fd_emitter)->get_nick() + " has changed topic to " + topic);
	return true;
}

bool	Channel::set_protected_topic(bool mode, int fd_emitter, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator");  
	this->_is_topic_protected = mode;
	if (this->_is_topic_protected == true)
		broadcast("#" + this->get_name() + "'s topic is now protected.");
	else
		broadcast("#" + this->get_name() + "'s topic is no longer protected.");
	return true;
}

bool	Channel::set_max_users(int max_users, int fd_emitter, t_data &data)
{

	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator");  
	if (max_users < 1)
		return error_feedback(fd_emitter, "Invalid max users number (must be >1)");
	this->_max_users = max_users;
	broadcast("#" + this->get_name() + "'s max users is now " + int_to_string(max_users) + " .");
	return true;
}


bool	Channel::set_has_user_limit(bool mode, int fd_emitter, t_data &data)
{
	if (is_op(fd_emitter) == false)
		return error_feedback(fd_emitter, "You are not channel operator");  
	this->_has_user_limit = mode;
	if (this->_has_user_limit == false)
		broadcast("#" + this->get_name() + " no longer has a user limit.");
	//TODO IF mode is TRUE, call set_max_users in the command_MODE() functtion
	return true;
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

void	Channel::broadcast(string message)
{
	vector<int>::iterator it = this->_fds_users.begin();
	vector<int>::iterator ite = this->_fds_users.end();

	message += "\n";
	for (; it != ite; it++)
	{
		write(*it, message.c_str(), message.size());
	}
}

bool	Channel::error_feedback(int fd_emitter, string message)
{
	message += "\n";
	write(fd_emitter, message.c_str(), message.size());
	return false;
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

string			Channel::get_name(void) const {return this->_name;};
vector<int>		Channel::get_users(void) const {return this->_fds_users;};
vector<int>		Channel::get_ops(void) const {return this->_fds_ops;};
vector<int>		Channel::get_invited(void) const {return this->_fds_invited;};
bool			Channel::get_invite_only(void) const {return this->_is_invite_only;};
string			Channel::get_topic(void) const {return this->_topic;}
bool			Channel::get_topic_set(void) const {return this->_is_topic_set ;};
bool			Channel::get_topic_protected(void) const {return this->_is_topic_protected ;};
string			Channel::get_key(void) const {return this->_key ;};
bool			Channel::get_channel_locked(void) const {return this->_is_channel_locked ;};
unsigned int	Channel::get_user_limit(void) const {return this->_max_users ;};
bool			Channel::get_has_user_limit(void) const {return this->_has_user_limit ;};

/******************************************************************************/
/*																		      */
/*                            END GETTERS		                              */
/*																		      */
/******************************************************************************/

