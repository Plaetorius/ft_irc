#include "Channel.hpp"
#include <iostream>
#include "User.hpp"
#include "messages.hpp"

/******************************************************************************/
/*																		      */
/*                                START RO3                                   */
/*																		      */
/******************************************************************************/

Channel::Channel()
{
	cout << "Default Channel constructor called" << endl;
}

/*
	The parameter name MUST NOT be empty. It has to be checked in the JOIN command
*/
Channel::Channel(string name, int fd_creator) : _is_invite_only(false), _is_topic_set(false), _is_topic_protected(false), _is_channel_locked(false), _max_users(0), _has_user_limit(false) 
{
	if (name[0] == '#')
		this->_name = name;
	else
		this->_name = "#" + name;
	this->add_user(fd_creator);
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

void	Channel::add_user(int fd_user)
{
	// if (this->_is_invite_only == true && is_invited(fd_user) == false)
	// 	return error_feedback(fd_user, "You must be invited to join this channel"); //User not invited && channel is invite-only
	// if (this->_max_users <= this->_fds_users.size())
	// 	return error_feedback(fd_user, "This channel is already full");
	// if (key != this->_key)
	// 	return error_feedback(fd_user, "Wrong password");
	// if (this->_is_invite_only == true)
	// 	this->_fds_invited.erase(find(this->_fds_invited.begin(), this->_fds_invited.end(), fd_user));

	//USEFULE FOR JOIN ^^^^^^^^
	if (this->_fds_users.empty())
		this->_fds_ops.push_back(fd_user);
	// cout << "^^^^^^^^^^^^^^^^^ADD USER^^^^^^^^^^^^^^^^^^^^^^" << endl;
	this->_fds_users.push_back(fd_user);
}

void	Channel::kick_user(int fd_to_kick)
{
	this->_fds_users.erase(find(this->_fds_users.begin(), this->_fds_users.end(), fd_to_kick));
}

void	Channel::part(int fd_user)
{
	this->_fds_users.erase(find(this->_fds_users.begin(), this->_fds_users.end(), fd_user));
}

void	Channel::op_user(int fd_to_op)
{
	if (is_op(fd_to_op) == false)
		this->_fds_ops.push_back(fd_to_op);
}

void	Channel::invite_user(int fd_to_invite)
{
	if (is_invited(fd_to_invite) == false)
		this->_fds_invited.push_back(fd_to_invite);
}

void	Channel::deop_user(int fd_to_deop)
{
	this->_fds_ops.erase(find(this->_fds_ops.begin(), this->_fds_ops.end(), fd_to_deop));
}

void	Channel::enable_locked_mode(string &key)
{
	this->_is_channel_locked = true;;
	this->_key = key;
}

void	Channel::disable_locked_mode()
{
	this->_is_channel_locked = false;
	this->_key.clear();
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

void	Channel::set_invite_only(bool mode)
{
	this->_is_invite_only = mode;
	if (mode == false)
		this->_fds_invited.clear();
}

void	Channel::set_topic(string topic)
{
	this->_is_topic_set = true;
	if (topic.empty() == true)
		this->_topic.clear();
	else
		this->_topic = topic;
}

void	Channel::set_protected_topic(bool mode)
{
	this->_is_topic_protected = mode;
}

void	Channel::set_max_users(int max_users)
{
	this->_max_users = max_users;
}

void	Channel::set_has_user_limit(bool mode)
{ 
	this->_has_user_limit = mode;
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

	message += "\r\n";
	for (; it != ite; it++)
	{
		// cout << "I sended to " << int_to_string(*it) << endl; 
		if (*it != fd_emitter)
			write(*it, message.c_str(), message.size());
	}
}

void	Channel::print_names(int target_fd)
{
	User	*target_user;
	User	*i_user;
	string	users_info = "";

	vector<int>::iterator users_fd_beg = _fds_users.begin();
	vector<int>::iterator users_fd_end = _fds_users.end();

	target_user = g_data_ptr->users[target_fd];
	while (users_fd_beg != users_fd_end)
	{
		i_user = g_data_ptr->users[*users_fd_beg];
		if (find(_fds_ops.begin(), _fds_ops.end(), *users_fd_beg) != _fds_ops.end())
			users_info += "@";
		users_info += i_user->get_nick();
		users_fd_beg++;
		if (users_fd_beg != users_fd_end)
			users_info += " ";
	}
	target_user->send_message(RPL_NAMREPLY(_name, target_user->get_nick(), target_user->get_user(), "localhost", users_info));
	target_user->send_message(RPL_ENDOFNAMES(_name, target_user->get_nick(), target_user->get_user(), "localhost"));
}

Channel *Channel::getChannel(string name)
{
    Channel *myChannel;

    try {
        myChannel = g_data_ptr->channels.at(name);
    }
    catch (const std::exception& e) {
        myChannel = NULL;
    }
    return (myChannel);
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

