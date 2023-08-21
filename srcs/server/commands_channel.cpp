#include "User.hpp"
#include "messages.hpp"
#include <sstream>
#include <ctype.h>

/**
 * @brief	Channel command
 * 
 * @link    https://modern.ircdocs.horse/#channel-operations
 * 
 * @attention   The code allow to add only 1 channel at a time
 * @attention	Doesn't support "0" parameter which says to quit everything
 * @attention	The code don't handle TOOMANYCHANNELS, BANNEDFROMCHAN
 */
bool	User::command_JOIN(t_command &command)
{
	Channel	*channel;
	string	channel_name;
	string	channel_key;

    /*  ********************************************************************* */
                                /*  Basic checks  */
    /*  ********************************************************************* */
	if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
	if (command.parameters.size() < 1) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "JOIN"));
		return false;
	}

	/*  ********************************************************************* */
							/*	Parse the channel info */
	/*  ********************************************************************* */
	channel_name = command.parameters.front();
    if (channel_name[0] != '#') {
        send_message(ERR_BADCHANMASK(channel_name));
        return false;
    }
	if (command.parameters.size() != 2)
		channel_key = "";
	else
		channel_key = command.parameters.back();
	channel = Channel::getChannel(channel_name);

	/*  ********************************************************************* */
						/*	Interract with the channel	*/
	/*  ********************************************************************* */
    if (channel)
	{
		if (channel->get_invite_only() == true) {
			send_message(ERR_INVITEONLYCHAN(_nick, channel_name));
			return false;
		}
		if (channel->get_has_user_limit() == true && channel->get_user_limit() == channel->get_users().size()) {
			send_message(ERR_CHANNELISFULL(_nick, channel_name));
			return (false);
		}
		if (channel->get_channel_locked() == true && channel->get_key() != channel_key) {
			send_message(ERR_BADCHANNELKEY(_nick, channel_name));
			return false;
		}
		if (channel->is_user(_fd) == true)
			return true;
		channel->add_user(_fd);
	}
	else
	{
		channel = new Channel(channel_name, _fd);
		send_message(CREATEDCHANNEL(channel_name));
		g_data_ptr->channels[channel_name] = channel;
	}
	channel->broadcast(JOIN(_nick, _user, "localhost",  channel_name), -1);
	if (channel->get_topic_set() == true) {
		send_message(RPL_TOPIC(_nick, _user, "localhost", channel_name, channel->get_topic()));
	}
	channel->print_names(_fd);
	_channels.push_back(channel);
	return true;
}

/**
 * @brief	[AFTER MODE COMMAND]	change or view the topic of the given channel
 * 
 * @example	TOPIC <channel> [<topic>]
 * @example		TOPIC #test :New topic  ; Setting the topic on "#test" to "New topic".
 * @example		TOPIC #test :           ; Clearing the topic on "#test"
 * @example		TOPIC #test             ; Checking the topic for "#test"
 * 
 * @link	https://modern.ircdocs.horse/#topic-message
 */
bool	User::command_TOPIC(t_command &command)
{
	/*  ********************************************************************* */
							/*	General checks	*/
	/*  ********************************************************************* */
	if (_is_identified == false)
	{
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	} else if (command.parameters.size() == 0) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "TOPIC"));
		return false;
	}


	/*  ********************************************************************* */
						/*	Get user information	*/
	/*  ********************************************************************* */
	string	channel_name = command.parameters.front();
	Channel	*channel = Channel::getChannel(channel_name);

	if (channel == NULL) {
		send_message(ERR_NOSUCHCHANNEL(_nick, channel_name));
		return false;
	} else if (channel->is_user(_fd) == false) {
		send_message(ERR_NOTONCHANNEL(channel_name));
		return false;
	}


	/*  ********************************************************************* */
							/*	Reading the topic	*/
	/*  ********************************************************************* */
	if (command.has_last_param == false)
	{
		if (channel->get_topic_set() == false) {
			send_message(RPL_NOTOPIC(_nick, _user, "localhost", channel_name));
		} else {
			send_message(RPL_TOPIC(_nick, _user, "localhost", channel_name, channel->get_topic()));
		}
		return true;
	}


	/*  ********************************************************************* */
							/*	Write the topic	*/
	/*  ********************************************************************* */


	if (channel->get_topic_protected() == true && channel->is_op(_fd) == false) {
		send_message(ERR_CHANOPRIVSNEEDED(user_id(_nick, _user, "localhost"), channel_name));
		return false;
	}
	if (command.last_param.size() <= 1) {
		channel->unset_topic();
		channel->broadcast(RPL_TOPIC2(_nick, _user, "localhost", channel_name, ""), -1);
	} else {
		channel->set_topic(command.last_param);
		channel->broadcast(RPL_TOPIC2(_nick, _user, "localhost", channel_name, command.last_param), -1);
	}
	return true;
}

/**
 * @brief	view the nicknames joined to a channel and membership prefixes
 * 
 * @param	<channel>{,<channel>}
 * 
 * @attention	one channel at a time
 */
bool	User::command_names(t_command &command)
{
	Channel	*target_channel;

	/*	Basic tests	*/
	if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
	if (command.parameters.size() == 0) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "NAMES"));
		return false;
	}
	
	target_channel = Channel::getChannel(command.parameters.front());
	if (target_channel == NULL) {
		send_message(ERR_NOSUCHCHANNEL(_nick, target_channel->get_name()));
		return false;
	}
	if (target_channel->is_user(_fd) == false)
	{
		send_message(ERR_NOTONCHANNEL(target_channel->get_name()));
		return false;
	}
	target_channel->print_names(_fd);
	return true;
}

/*  ********************************************************************* */
							/*	List command?	*/
/*  ********************************************************************* */

/**
 * @brief	invite a user to a channel
 * 
 * @example	INVITE	<nickname> <channel>
 * @example		INVITE Wiz #foo_bar    					; Invite Wiz to #foo_bar
 * @example		 :dan-!d@localhost INVITE Wiz #test		; dan- has invited Wiz to the channel #test
 * 
 * @attention	Doesn't handle no parameter(to show all channels where it was invited)
 * @link		https://modern.ircdocs.horse/#invite-message
 */
bool	User::command_INVITE(t_command &command)
{
	/*	Basic tests	*/
		/*	If not authenticated	*/
	if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
		/*	If not enough parameters	*/
	if (command.parameters.size() != 2)
	{
		send_message(ERR_NEEDMOREPARAMS(_nick, "INVITE"));
		return false;
	}

	/*	If channel doesn't exist	*/
		/*	ERR_NOSUCHCHANNEL	*/
		/*	Exit	*/
	string channel_name = command.parameters.at(1);
	t_channels::iterator it_channels = g_data_ptr->channels.find(channel_name);
	if (it_channels == g_data_ptr->channels.end())
	{
		send_message(ERR_NOSUCHCHANNEL(_nick, channel_name));
		return false;
	}
	Channel *channel = it_channels->second;

	/*	If target user doesn't exist	*/
		/*	CUSTOM_MESSAGE	*/
		/*	EXIT	*/
	t_users::iterator it_users = g_data_ptr->users.begin();	
	t_users::iterator ite_users = g_data_ptr->users.end();	
	for (; it_users != ite_users; it_users++)
	{
		if (it_users->second->get_nick() == command.parameters.at(0))
			break; 
	}
	if (it_users == ite_users)
	{
		send_message(ERR_NOSUCHNICKCHANNEL(command.parameters.at(0)));
		return false;
	}
	User *invited_user = it_users->second;
	/*	If the current user not on channel	*/
		/*	ERR_NOTONCHANNEL	*/
		/*	EXIT	*/
	if (channel->is_user(_fd) == false)
	{
		send_message(ERR_NOTONCHANNEL(channel_name));
		return false;
	}
	/*	If the mode is invite only and user not operator	*/
		/*	ERR_CHANOPRIVSNEEDED	*/
		/*	EXIT	*/
	if (channel->get_invite_only() == true && channel->is_op(_fd) == false)
	{
		send_message(ERR_CHANOPRIVSNEEDED(user_id(_nick, _user, "localhost"), channel_name));
		return false;	
	}
	/*	If user is on channel	*/
		/*	ERR_USERONCHANNEL	*/
		/*	EXIT	*/
	if (channel->is_user(invited_user->get_fd()) == true)
	{
		send_message(ERR_USERONCHANNEL(_nick, invited_user->get_nick(), channel_name));
		return false;
	}
	/*	Send RPL_INVITING to current user	*/
	/*	Send INVITE to target user	*/
	send_message(RPL_INVITING(_nick, _user, _name, invited_user->get_nick(), channel_name));
	channel->invite_user(invited_user->get_fd());
	send_message(INVITE(_nick, _user, _name, invited_user->get_nick(), channel_name));
	return true;
}

/**
 * @brief	removes the user from the given channel(s)
 * 
 * @example		PART #twilight_zone             ; dan- is leaving the channel "#twilight_zone"
 * @example		PART #twilight_zone "AFK"		; dan- is leaving the channel "#twilight_zone": AFK
 * 
 * @attention	Works one part at a time
 * @link		https://modern.ircdocs.horse/#part-message
 */
bool	User::command_PART(t_command &command)
{
	/*  ********************************************************************* */
                                /*  Basic checks  */
    /*  ********************************************************************* */
	/*	Basic tests	*/
		/*	If not authenticated	*/
	if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
		/*	If not enough parameters	*/
	if (command.parameters.size() == 0) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "PART"));
		return false;
	}

	/*	Check the channel	*/

		/*	If channel doesn't exist  */
			/*	ERR_NOSUCHCHANNEL	*/
	string	channel_name = command.parameters.front(); 
	Channel	*channel = Channel::getChannel(channel_name);
	
	if (channel == NULL) {
		send_message(ERR_NOSUCHCHANNEL(_nick, channel_name));
		return false;
	}
	if (channel->is_user(_fd) == false) {
		send_message(ERR_NOTONCHANNEL(channel_name));
		return false;
	}
	/*	If OK	*/
		/*	Notify everybody that client quitted the channel  */
	if (command.has_last_param == false) {
		channel->broadcast(PART_WOREASON(_nick, _user, "localhost", channel_name), -1);
	} else {
		// cout << "I was in WREASON! " << PART_WREASON(_nick, _user, "localhost", channel_name, command.last_param) << endl;
		channel->broadcast(PART_WREASON(_nick, _user, "localhost", channel_name, command.last_param), -1);
	}
	channel->part(_fd);
	_channels.erase(find(_channels.begin(), _channels.end(), channel));
	/* If channel empty, remove it */
	if (channel->get_users().empty() == true)
	{
		g_data_ptr->channels.erase(channel->get_name());
		delete channel;
	}
	return true;
}

/**
 * @brief	request the forced removal of a user from a channel
 * 
 * @example		KICK <channel> <user> *( "," <user> ) [<comment>]
 * 
 * @attention	One user at a time
 * @link		https://modern.ircdocs.horse/#kick-message
 */
bool	User::command_KICK(t_command &command)
{
	/*	********************************************************************* */
								/*	Basic tests	*/
	/*	********************************************************************* */
	if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
	if (command.parameters.size() < 2) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "KICK"));
		return false;
	}

	/*	********************************************************************* */
				/*	Get the information from the user	*/
	/*	********************************************************************* */
	string	channel_name = command.parameters.at(0);
	string	target_name	= command.parameters.at(1);
	Channel	*channel = Channel::getChannel(channel_name);
	User	*target_user = User::getUser(target_name, server);

	if (channel == NULL) {
		send_message(ERR_NOSUCHCHANNEL(_nick, channel_name));
		return false;
	}
	if (target_user == NULL) {
		send_message(ERR_NOSUCHNICKCHANNEL(command.parameters.at(1)));
		return false;
	}
	if (channel->is_user(_fd) == false) {
		send_message(ERR_NOTONCHANNEL(channel_name));
		return false;
	}
	if (channel->is_op(_fd) == false) {
		send_message(ERR_CHANOPRIVSNEEDED(user_id(_nick, _user, "localhost"), channel_name));
		return false;
	}
	if (channel->is_user(target_user->get_fd()) == false) {
		send_message(ERR_USERNOTINCHANNEL(target_user->get_nick(), channel_name));
		return false;
	}
	
	/*	********************************************************************* */
					/*	Kick the user from the channel	*/
	/*	********************************************************************* */

	if (command.has_last_param == true) {
		channel->broadcast(RPL_KICK2(user_id(_nick, _user, "localhost"), channel_name, target_name, command.last_param), -1);
	} else {
		channel->broadcast(RPL_KICK2(user_id(_nick, _user, "localhost"), channel_name, target_name, "Don't like your name"), -1);
	}
	channel->kick_user(target_user->get_fd());
	target_user->remove_channel(channel_name);

	//If channel empty, remove Channel
	if (channel->get_users().empty() == true)
	{
		// g_data_ptr->channels.erase(find(g_data_ptr->channels.begin(), g_data_ptr->channels.end(), channel->get_name()));
		_channels.erase(find(_channels.begin(), _channels.end(), channel));
		delete channel;
	}
	return true;
}

/**
 * @brief	set or remove options (or modes) from a given target
 * 
 * @example	<target> [<modestring> [<mode arguments>...]]
 * 
 * @attention	The User mode has only +o mode
 * @attention	The channel mode has +i, 
 */
bool	User::command_MODE(t_command &command)
{
	/*	Basic tests	*/
		/*	If not authenticated	*/
	if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
		/*	If not enough parameters	*/
	if (command.parameters.size() < 1)
	{
		send_message(ERR_NEEDMOREPARAMS(_nick, "MODE"));
		return false;
	}
	if (command.parameters.size() == 1)
	{
		string	myBeautifulString;

		myBeautifulString = command.parameters.front();
		if (myBeautifulString[0] == '#')
		{
			Channel *smartChannel;

			smartChannel = Channel::getChannel(myBeautifulString);
			if (smartChannel == NULL)
			{
				send_message(ERR_NOSUCHCHANNEL(_nick, command.parameters.front()));
				return false;
			}
			send_message(RPL_CHANNELMODEIS(_nick, smartChannel->get_name()));
			return true;
		}
		else
		{
			User	*sportyUser;

			sportyUser = User::getUser(myBeautifulString, g_data_ptr);
			if (sportyUser == NULL)
			{
				send_message(ERR_NOSUCHNICKCHANNEL(myBeautifulString));
				return false;
			}
			send_message(RPL_UMODEIS(myBeautifulString));
			return true;
		}
	}
	if (command.parameters.front()[0] != '#')
		return true;

	// I added this, because the +i commands takes a nickname and not a channelname as a first paramater, yes 

	/*	If channel doesn't exist	*/
		/*	ERR_NOSUCHCHANNEL	*/
		/*	Exit	*/
	string channel_name = command.parameters.at(0);
	t_channels::iterator it_channels = g_data_ptr->channels.find(channel_name);
	if (it_channels == g_data_ptr->channels.end())
	{
		send_message(ERR_NOSUCHCHANNEL(_nick, channel_name));
		return false;
	}
	Channel *channel = it_channels->second;
	/*	Check if the emitter is OP */
	if (channel->is_op(_fd) == false)
	{
		send_message(ERR_NOPRIVILEGES(_nick));
		return false;
	}
	/* Identifying the MODE requested */
	string mode = command.parameters.at(1);
	if (mode == "+i")
	{
		channel->set_invite_only(true);
		channel->broadcast(channel_name + ": is now invite-only.", _fd);
	}
	else if (mode == "-i")
	{
		channel->set_invite_only(false);
		channel->broadcast(channel_name + ": is no longer invite-only.", _fd);
	}
	else if (mode == "+t")
	{
		channel->set_protected_topic(true);
		channel->broadcast(channel_name + ": topic is now protected.", _fd);
	}
	else if (mode == "-t")
	{
		channel->set_protected_topic(false);
		channel->broadcast(channel_name + ": topic is no longer protected.", _fd);
	}
	else if (mode == "+k")
	{
		if (command.parameters.size() != 3)
		{
			ERR_NEEDMOREPARAMS(_nick, "MODE");
			return false;
		}
		if (channel->get_channel_locked() == true)
		{
			send_message(ERR_KEYSET(channel_name));
			return false;
		}
		if (command.parameters.at(2).empty())
		{
			channel->disable_locked_mode();
			channel->broadcast(channel_name + ": is no longer locked.", _fd);
		}
		else
		{
			channel->enable_locked_mode(command.parameters.at(2));
			channel->broadcast(channel_name + ": is now locked.", _fd);
		}
	}
	else if (mode == "-k")
	{
		if (command.parameters.size() != 3)
		{
			ERR_NEEDMOREPARAMS(_nick, "MODE");
			return false;
		}
		if (channel->get_channel_locked() == true)
		{
			send_message(ERR_KEYSET(channel_name));
			return false;
		}
		if (channel->get_key() != command.parameters.at(2))
		{
			send_message(ERR_BADCHANNELKEY(_nick, channel_name));
			return false;
		}
		channel->disable_locked_mode();
		channel->broadcast(channel_name + ": is no longer locked.", _fd);
	}
	else if (mode == "+o")
	{
		if (command.parameters.size() != 3)
		{
			ERR_NEEDMOREPARAMS(_nick, "MODE");
			return false;
		}
		t_users::iterator it_users = g_data_ptr->users.begin();	
		t_users::iterator ite_users = g_data_ptr->users.end();	
		for (; it_users != ite_users; it_users++)
		{
			if (it_users->second->get_nick() == command.parameters.at(2))
				break; 
		}
		if (it_users == ite_users)
		{
			send_message(ERR_NOSUCHNICKCHANNEL(command.parameters.at(2)));
			return false;
		}
		User *to_op_user = it_users->second;
		if (channel->is_user(to_op_user->get_fd()) == false)
		{
			string message = "Error: " + to_op_user->get_nick() + " isn't a member of the channel " + channel_name + "\r\n";
			write(_fd, message.c_str(), message.size());
			return false;
		}
		if (channel->is_op(to_op_user->get_fd()) == false)
			channel->op_user(to_op_user->get_fd());
		channel->broadcast(channel_name + ": " + to_op_user->get_nick() + " is now channel operator.", _fd);
		return true;
	}
	else if (mode == "-o")
	{
		if (command.parameters.size() != 3)
		{
			ERR_NEEDMOREPARAMS(_nick, "MODE");
			return false;
		}
		t_users::iterator it_users = g_data_ptr->users.begin();	
		t_users::iterator ite_users = g_data_ptr->users.end();	
		for (; it_users != ite_users; it_users++)
		{
			if (it_users->second->get_nick() == command.parameters.at(2))
				break; 
		}
		if (it_users == ite_users)
		{
			send_message(ERR_NOSUCHNICKCHANNEL(command.parameters.at(2)));
			return false;
		}
		User *to_deop_user = it_users->second;
		if (channel->is_op(to_deop_user->get_fd()) == true)
			channel->deop_user(to_deop_user->get_fd());
		channel->broadcast(channel_name + ": " + to_deop_user->get_nick() + " is no longer channel operator.", _fd);
		return true;
	}
	else if (mode == "+l")
	{
		if (command.parameters.size() != 3)
		{
			ERR_NEEDMOREPARAMS(_nick, "MODE");
			return false;
		}
		int value = atoi(command.parameters.at(2).c_str());
		if (value < 1)
			return false;
		channel->set_max_users(value);
		channel->set_has_user_limit(true);
		channel->broadcast(channel_name + ": is now limited in members " + int_to_string(value) + ".", _fd);
	}
	else if (mode == "-l")
	{
		channel->set_has_user_limit(false);
		channel->broadcast(channel_name + ": is no longer limited in members.", _fd);
	}
	else
	{
		write(_fd, "Unknow mode.\r\n", 15);
		return false;
	}
	return true;

	//FOR MODE L
	// if (is_op(fd_emitter) == false)
	// 	return error_feedback(fd_emitter, "You are not channel operator"); 
	//	if (max_users < 1)
	//	return error_feedback(fd_emitter, "Invalid max users number (must be >1)");
	//	broadcast("#" + this->get_name() + "'s max users is now " + int_to_string(max_users) + " .");

	// if (this->_has_user_limit == false)
	// broadcast("#" + this->get_name() + " no longer has a user limit.");
	//TODO IF mode is TRUE, call set_max_users in the command_MODE() functtion

	/*	Just the values	*/
}
// /connect localhost 6667 test