#include "User.hpp"
#include "Channel.hpp"
#include "messages.hpp"
#include <sstream>
#include <ctype.h>

/**
 * ****************************************************************************
 * @brief       send a password to the server during the connection process.
 * 
 * @return      I don't even know why do I need it
 * @link        https://modern.ircdocs.horse/#pass-message
 * 
 * @bug         if multiple params, I take only first
 * @bug         I suppose, the password couldn't passes inside of last_param
 * ****************************************************************************
 */
bool	User::command_PASS(t_command &command)
{
    if (_has_password)
        send_message(ERR_ALREADYREGISTERED(int_to_string(_id)));
    else if (command.parameters.size() == 0)
        send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "PASS"));
    else if (command.parameters.front() == server->password)
        _has_password = true;
    else if (command.parameters.front() != server->password)
        send_message(ERR_PASSWDMISMATCH(int_to_string(_id)));
    else
        send_message("fatal");
    return (true);
}

/**
 * ****************************************************************************
 * @brief       give the client a nickname or change the previous one
 * 
 * @link        https://modern.ircdocs.horse/#nick-message 
 * @link        https://modern.ircdocs.horse/#connection-registration a proper registration
 * 
 * @bug         if multiple params, I take only first
 * @bug         I assume that last_param doesn't contain the NICK param
 * @bug         Doesn't understand what does it mean: 
 *                      Servers MAY allow extra characters, as long as they 
 *                      do not introduce ambiguity in other commands
 * ****************************************************************************
 */
bool	User::command_NICK(t_command &command)
{
	/*  ********************************************************************* */
				/*	Check if password is set	*/
	/*  ********************************************************************* */
	if (!_has_password) {
		send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
		return false;
	}

    /*  ********************************************************************* */
    /*  Check if proper number of parameters    */
    /*  ********************************************************************* */
    if (command.parameters.size() == 0){
        if (_has_nick == 1) {
            send_message(ERR_NONICKNAMEGIVEN(_nick));
        } else {
            send_message(ERR_NONICKNAMEGIVEN(int_to_string(this->_id)));
        }
        return false;
    }

    /*  ********************************************************************* */
    /*  Check if the Nickname is properly formatted */
    /*  ********************************************************************* */
    std::string param = command.parameters.front();
    char        firstChar = param[0];

    /*  Check first */
    if (!(isalpha(firstChar) || firstChar == '[' || firstChar == ']' || firstChar == '{' || firstChar == '}' ||  firstChar == '\\' || firstChar == '|'))
    {
        if (_has_nick == 1) {
            send_message(ERR_ERRONEUSNICKNAME(_nick, param));
        } else {
            send_message(ERR_ERRONEUSNICKNAME(int_to_string(this->_id), param));
        }
        return false;
    }

    /*  Check consecutive */
    for (size_t i = 1; i < param.length(); ++i)
    {
        char ch = param[i];
        if (!(isalnum(ch) || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '\\' || ch == '|')) {
            send_message(ERR_ERRONEUSNICKNAME(int_to_string(this->_id), param));
            return false;
        }
    }

    /*  ********************************************************************* */
                    /*  Check if the nick already exists    */
    /*  ********************************************************************* */
    for (size_t i = 0; i < g_data_ptr->open_fds.size(); i++)
    {
        if (g_data_ptr->users[g_data_ptr->open_fds[i]]->get_nick() == param) {
            if (g_data_ptr->open_fds[i] != this->_fd) {
                param = param + "_";
            }
        }
    }

    /*  ********************************************************************* */
                /*  Inform others about the change of name */
    /*  ********************************************************************* */
    if (this->_has_nick)
    {
        for (size_t i = 0; i < g_data_ptr->open_fds.size(); i++)
        {
            User    *user = g_data_ptr->users[g_data_ptr->open_fds[i]];
            if (user->get_identification() == true)
                user->send_message(CHANGE_NICKNAME(_nick, _user, "localhost", param));
        }
    }

    /*  ********************************************************************* */
                        /*  Assign the nickname */
    /*  ********************************************************************* */
    this->_nick = param;
    _has_nick = true;
    if (this->_has_user && this->_has_password && !this->_is_identified)
    {
        this->_is_identified = true;
        this->send_message(RPL_WELCOME(_nick, _user, int_to_string(g_data_ptr->port), "localhost"));
    }
    return true;
}

/**
 * @brief   specify the username and realname of a new user.
 * 
 * @return  Why do I need it?
 * @link    https://modern.ircdocs.horse/#user-message
 * 
 * @bug     In the protocol, the username could be stated by IDENT protocol
 *          https://datatracker.ietf.org/doc/html/rfc1413 I didn't implement it
 * @bug		It is said that the command should contain 0 or *, but I didn't understand why
 */
bool	User::command_USER(t_command &command)
{
	/*	********************************************************************* */
								/*	Small checks	*/
	/*	********************************************************************* */
	if (!_has_password) {
        if (_has_nick == 1) {
		    send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
        return false;
	}
	if (command.parameters.size() == 0) {
        if (_has_nick == 1) {
            send_message(ERR_NEEDMOREPARAMS(_nick, "USER"));
        } else {
    	    send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "USER"));
        }
        return false;
	}
	if (_has_user) {
        if (_has_nick == 1) {
            send_message(ERR_ALREADYREGISTERED(_nick));
        } else {
    	    send_message(ERR_ALREADYREGISTERED(int_to_string(_id)));
        }
        return false;
	}

	/*	********************************************************************* */
							/*	Set the username	*/
	/*	********************************************************************* */
	this->_user = command.parameters.front();
	this->_name = command.last_param;
	_has_user = true;
	if (_has_user && _has_nick && !_is_identified)
	{
		this->_is_identified = true;
		send_message(RPL_WELCOME(_nick, _user, int_to_string(g_data_ptr->port), "localhost"));
	} 
	return true;
}

/**
 * @brief 	check if the server connection is still connected 
 * 
 * @link	https://modern.ircdocs.horse/#ping-message 
 */
bool	User::command_PING(t_command &command)
{
	/*	********************************************************************* */
					/*	Check the number of parameters	*/
	/*	********************************************************************* */
	// if (command.parameters.size() == 0) {
    //     if (_has_nick == 1) {
    //         send_message(ERR_NEEDMOREPARAMS(_nick, "PING"));
    //     } else {
    // 	    send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "PING"));
    //     }
    //     return false;
	// }

	/*	********************************************************************* */
						/*	Answer to the request	*/
	/*	********************************************************************* */
	if (_is_identified == false) {
        if (command.parameters.size() == 0) {
            send_message(PING(int_to_string(_id), ""));
        } else {
            send_message(PING(int_to_string(_id), command.parameters.front()));
        }
    } else {
        if (command.parameters.size() == 0) {
            send_message(PONG(user_id(_nick, _user, "localhost"), ""));
        } else {
    		send_message(PONG(user_id(_nick, _user, "localhost"), command.parameters.front()));
        }
    }
    return true;
}

/**
 * @brief	 obtain IRC operator privileges
 * 
 * @param   <name> <password>
 * 
 * @link	https://modern.ircdocs.horse/#oper-message
 */
bool	User::command_OPER(t_command &command)
{
	/*	********************************************************************* */
							/*	Basic checks	*/
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
		send_message(ERR_NEEDMOREPARAMS(_nick, "OPER"));
		return false;
	}

	/*	********************************************************************* */
						/*	Check the user and password	*/
	/*	********************************************************************* */
	if (command.parameters.front() != LOGIN || command.parameters.back() != PASSWORD) {
		send_message(ERR_PASSWDMISMATCH(_nick));
		return false;
	}

	send_message(RPL_YOUREOPER(_nick));
    server->operator_fds.push_back(_fd);
	return true;
}

/**
 * @brief   close the connection between a given client and the server
 * 
 * @param       <target nickname> <comment>
 * @return      the fd of the target user
 * 
 * @link        https://modern.ircdocs.horse/#kill-message
 * @attention   the user has to be deleted from the server side
 */
int		User::command_KILL(t_command &command)
{
    User    *target_user;
    string  target_nick;

    /*	********************************************************************* */
                                /*  Basic tests */
    /*	********************************************************************* */
    if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
	if (command.parameters.size() < 1) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "KILL"));
		return -1;
	}
    if (is_operator() == false) {
        send_message(ERR_NOPRIVILEGES(_nick));
        return -1;
    }

    /*	********************************************************************* */
                /*  Get information about the target user   */
    /*	********************************************************************* */
    target_nick = command.parameters.front();
    target_user = User::getUser(target_nick, server);
    if (target_user == NULL) {
        send_message(ERR_NOSUCHNICKCHANNEL(target_nick));
        return -1;
    }

    /*	********************************************************************* */
            /*  Go through the channels and remove the target user  */
    /*	********************************************************************* */
    vector<Channel *>::iterator iter_beg = _channels.begin();
    vector<Channel *>::iterator iter_end = _channels.end();
    while (iter_beg != iter_end)
    {
        Channel *myChannel = *iter_beg;

        myChannel->kick_user(target_user->get_fd());
        target_user->remove_channel(myChannel->get_name());
        if (myChannel->get_users().size() != 0)
        {
            if (command.has_last_param == 1) {
                myChannel->broadcast(QUIT2(user_id(target_nick, target_user->get_user(), "localhost"), command.last_param), -1);
            } else {
                myChannel->broadcast(QUIT2(user_id(target_nick, target_user->get_user(), "localhost"), "default reason"), -1);
            }
        } else {   // No more users left in the channel
            this->server->channels.erase(myChannel->get_name());
            delete myChannel;
        }
        iter_beg++;
    }

    /*	********************************************************************* */
                    /*  Notify the user that he was killed  */
    /*	********************************************************************* */
    if (command.has_last_param == true) {
        target_user->send_message(KILL(_nick, command.last_param));
    } else {
        target_user->send_message(KILL(_nick, "default reason"));
    }
    return (target_user->get_fd());
}

/**
 * @brief       quit the server
 * 
 * @param       [<reason>]
 * @return      the fd of a user
 * 
 * @link        https://modern.ircdocs.horse/#quit-message
 */
int     User::command_QUIT(t_command &command)
{
    /*	********************************************************************* */
            /*  Go through the channels and remove the target user  */
    /*	********************************************************************* */
    vector<Channel *>::iterator iter_beg = _channels.begin();
    vector<Channel *>::iterator iter_end = _channels.end();
    while (iter_beg != iter_end)
    {
        Channel *myChannel = *iter_beg;

        myChannel->kick_user(_fd);
        _channels.erase(iter_beg);
        if (myChannel->get_users().size() != 0) // There is someone in the channel
        {
            if (command.last_param.size() != 0)
                myChannel->broadcast(QUIT2(user_id(_nick, _user, "localhost"), command.last_param), -1);
            else
                myChannel->broadcast(QUIT2(user_id(_nick, _user, "localhost"), "default reason"), -1);
        }
        else    // No more users left in the channel
        {
            this->server->channels.erase(myChannel->get_name());
            delete myChannel;
        }
        iter_beg++;
    }
    // this->server->open_fds.erase(find(server->open_fds.begin(), server->open_fds.end(), _fd));
    return (_fd);
}

/**
 * @brief   send private messages between users
 * 
 * @param   <receiver>{,<receiver>} <text to be sent> 
 * @example PRIVMSG Angel :yes I'm receiving it !       ; Command to send a message to Angel.
 * @example PRIVMSG %#bunny :Hi! I have a problem!      ; Command to send a message to halfops and chanops on #bunny.
 * 
 * @link        https://modern.ircdocs.horse/#privmsg-message 
 * @attention   In general, the irc handles privmsg to users with specific
 *              host mask and server mask. This things wasn't integrated 
 *              in the code. But channel mask were handled
 * @attention   I didn't handle the wildcards
 * @attention   I didn't handle several users
 * 
 * @bug         Do I need to handle the banned from channel cases?
 */
bool	User::command_PRIVMSG(t_command &command)
{
    /*  Basic tests */
    if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
	if (command.parameters.size() < 1 || (command.parameters.size() == 1 && command.last_param.size() == 0)) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "PRIVMSG"));
		return false;
	}

    std::string target = command.parameters.front();

    /*  if param is channel */
    if (target[0] == '#')
    {
        Channel *target_channel = Channel::getChannel(target);

        /*  If channel doesn't exist    */ /* Error */
        if (!target_channel) {
       		send_message(ERR_NOSUCHCHANNEL(_nick, target));
            return false;
        }

        //  I need to check here if I am in the channel!!!!!!!!!!!!
        if (target_channel->is_user(_fd) == false) {
            send_message(ERR_NOTONCHANNEL(target));
            return false;
        }
        target_channel->broadcast(PRIVMSG2(_nick, _user, "localhost", target, command.last_param), _fd);
    }
    /*  else param is user  */
    else
    {
        /*  If the target user doesn't exist */
        User    *target_user = User::getUser(target, server);

        if (!target_user) {
            send_message(ERR_NOSUCHNICKCHANNEL(target));
            return false;
        }

        /*  Send the message    */
        target_user->send_message(PRIVMSG(_nick, _user, "localhost", target, command.last_param));
    }
    return true;
}

/**
 * @brief   For our project is fully the same
 * 
 * @param   <target>{,<target>} <text to be sent>
 * 
 * @link        https://modern.ircdocs.horse/#notice-message
 * @attention   I didn't handle several users
 */
bool	User::command_NOTICE(t_command &command)
{
    /*  Basic tests */
    if (_is_identified == false) {
        if (_has_nick == 1) {
    		send_message(ERR_NOPRIVILEGES(_nick));
        } else {
            send_message(ERR_NOPRIVILEGES(int_to_string(_id)));
        }
    	return false;
	}
	if (command.parameters.size() < 1 || (command.parameters.size() == 1 && command.last_param.size() == 0)) {
		send_message(ERR_NEEDMOREPARAMS(_nick, "NOTICE"));
		return false;
	}

    std::string target = command.parameters.front();

    /*  if param is channel */
    if (target[0] == '#')
    {
        Channel *target_channel = Channel::getChannel(target);

        /*  If channel doesn't exist    */ /* Error */
        if (!target_channel) {
       		send_message(ERR_NOSUCHCHANNEL(_nick, target));
            return false;
        }
        /*  Broadcast to everybody  */
        target_channel->broadcast(PRIVMSG2(_nick, _user, "localhost", target, command.last_param), _fd);
    }
    /*  else param is user  */
    else
    {
        /*  If the target user doesn't exist */
        User    *target_user = User::getUser(target, server);

        if (!target_user) {
            send_message(ERR_NOSUCHNICKCHANNEL(target));
            return false;
        }
    
        /*  Send the message    */
        target_user->send_message(PRIVMSG(_nick, _user, "localhost", target, command.last_param));
    }
    return true;
}

bool     User::command_unknown(t_command &command)
{
    cout << "Was I here?" << endl;
    if (_has_nick)
        send_message(ERR_UNKNOWNCOMMAND(_nick, command.command));
    else
        send_message(ERR_UNKNOWNCOMMAND(int_to_string(_id), command.command));
    return (false);

}
