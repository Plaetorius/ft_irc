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
		send_message(PERMISSIONDENIED);
		return false;
	}

    /*  ********************************************************************* */
    /*  Check if proper number of parameters    */
    /*  ********************************************************************* */
    if (command.parameters.size() == 0)
    {
        send_message(ERR_NONICKNAMEGIVEN(int_to_string(this->_id)));
        return false;
    }

    /*  ********************************************************************* */
    /*  Check if the Nickname is properly formatted */
    /*  ********************************************************************* */
    std::string param = command.parameters.front();
    char        firstChar = param[0];

    /*  Check first */
    if (!(isalpha(firstChar) || firstChar == '[' || firstChar == ']' || 
                                firstChar == '{' || firstChar == '}' || 
                                firstChar == '\\' || firstChar == '|'))
    {
        send_message(ERR_ERRONEUSNICKNAME(int_to_string(this->_id), param));
        return false;
    }

    /*  Check consecutive */
    for (size_t i = 1; i < param.length(); ++i)
    {
        char ch = param[i];
        if (!(isalnum(ch) || ch == '[' || ch == ']' || ch == '{' 
                            || ch == '}' || ch == '\\' || ch == '|'))
        {
            send_message(ERR_ERRONEUSNICKNAME(int_to_string(this->_id), param));
            return false;
        }
    }

    /*  ********************************************************************* */
                    /*  Check if the nick already exists    */
    /*  ********************************************************************* */
    for (int i = 0; i < g_data_ptr->open_fds.size(); i++)
    {
        if (g_data_ptr->users[g_data_ptr->open_fds[i]]->get_nick() == param)
        {
            if (g_data_ptr->open_fds[i] != this->_fd)
            {
                send_message(ERR_NICKNAMEINUSE(int_to_string(this->_id), param));
                return (false);
            }
        }
    }

    /*  ********************************************************************* */
                /*  Inform others about the change of name */
    /*  ********************************************************************* */
    if (this->_has_nick)
    {
        for (int i = 0; i < g_data_ptr->open_fds.size(); i++)
        {
            User    *user = g_data_ptr->users[g_data_ptr->open_fds[i]];
            if (user->get_identification() == true && user->get_fd() != this->_fd)
                user->send_message(CHANGE_NICKNAME(user->get_nick(), param));
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
        this->send_message(RPL_WELCOME(int_to_string(_id), _nick, _user, 
                                        int_to_string(g_data_ptr->port)));
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
		send_message(PERMISSIONDENIED);
		return false;
	}
	if (command.last_param.empty() == true || command.parameters.size() == 0) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "USER"));
		return false;
	}
	if (_has_user) {
		send_message(ERR_ALREADYREGISTERED(int_to_string(_id)));
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
		send_message(RPL_WELCOME(int_to_string(_id), _nick, _user, 
                                    int_to_string(g_data_ptr->port)));
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
	if (command.parameters.size() == 0) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "PING"));
		return false;
	}

	/*	********************************************************************* */
						/*	Answer to the request	*/
	/*	********************************************************************* */
	if (_is_identified == false)
		send_message("PING :" + command.parameters.front() + "\r\n");
	else
		send_message("PONG :" + command.parameters.front() + "\r\n");
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
		send_message(PERMISSIONDENIED);
		return false;
	}
	if (command.parameters.size() < 2) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "OPER"));
		return false;
	}

	/*	********************************************************************* */
						/*	Check the user and password	*/
	/*	********************************************************************* */
	if (command.parameters.front() != LOGIN || command.parameters.back() != PASSWORD)
	{
		send_message(ERR_PASSWDMISMATCH(int_to_string(_id)));
		return false;
	}

	send_message(RPL_YOUREOPER(int_to_string(this->_id)));
    server->operator_fds.push_back(_id);
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
    bool    has_reason;

    /*	********************************************************************* */
                                /*  Basic tests */
    /*	********************************************************************* */
    if (_is_identified == false) {
		send_message(PERMISSIONDENIED);
		return false;
	}
	if (command.parameters.size() < 1) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "KILL"));
		return false;
	}
    if (is_operator() == false) {
        send_message(ERR_NOPRIVILEGES(_nick));
        return false;
    }

    /*	********************************************************************* */
                /*  Get information about the target user   */
    /*	********************************************************************* */
    target_nick = command.parameters.front();
    target_user = User::getUser(target_nick, server);
    if (target_user == NULL) {
        send_message(ERR_NOSUCHNICKCHANNEL(target_nick));
        return false;
    }
    if (command.last_param.size() == 0)
        has_reason = 0;
    else
        has_reason = 1;

    /*	********************************************************************* */
            /*  Go through the channels and remove the target user  */
    /*	********************************************************************* */
    vector<Channel *>::iterator iter_beg = _channels.begin();
    vector<Channel *>::iterator iter_end = _channels.end();
    while (iter_beg != iter_end)
    {
        Channel *myChannel = *iter_beg;

        myChannel->kick_user(target_user->get_fd());
        if (myChannel->get_users().size() != 0)
        {
            if (has_reason == 1)
                myChannel->broadcast(QUIT_WREASON(target_nick, command.last_param));
            else
                myChannel->broadcast(QUIT_WOREASON(target_nick));
        }
        else    // No more users left in the channel
        {
            this->_channels.erase(iter_beg);
            this->server->channels.erase(myChannel->get_name());
            delete myChannel;
        }
        iter_beg++;
    }

    /*	********************************************************************* */
                    /*  Notify the user that he was killed  */
    /*	********************************************************************* */
    if (has_reason == true)
        target_user->send_message(KILL_WREASON(_nick, command.last_param));
    else
        target_user->send_message(KILL_WOREASON(_nick));

    // this->server->open_fds.erase(find(server->open_fds.begin(), server->open_fds.end(), target_user->get_fd()));
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
        if (myChannel->get_users().size() != 0) // There is someone in the channel
        {
            if (command.last_param.size() != 0)
                myChannel->broadcast(QUIT_WREASON(_nick, command.last_param));
            else
                myChannel->broadcast(QUIT_WOREASON(_nick));
        }
        else    // No more users left in the channel
        {
            this->_channels.erase(iter_beg);
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
		send_message(PERMISSIONDENIED);
		return false;
	}
	if (command.parameters.size() < 1) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "PRIVMSG"));
		return false;
	}

    std::string param_str = command.parameters.front();

    /*  if param is channel */
    if (param_str[0] == '#')
    {
        Channel *myChannel = Channel::getChannel(param_str.substr(1));

        /*  If channel doesn't exist    */ /* Error */
        if (!myChannel)
       		send_message(ERR_NOSUCHCHANNEL(int_to_string(_id), param_str));
        else
            /*  Broadcast to everybody  */
            myChannel->broadcast(command.last_param + "\r\n");
    }
    /*  else param is user  */
    else
    {
        /*  If the target user doesn't exist */
        User    *myUser = User::getUser(param_str, server);

        if (!myUser) {
            send_message(ERR_NOSUCHNICKCHANNEL(param_str));
            return false;
        }

        /*  Send the message    */
        myUser->send_message(PRIVMSG(_nick, command.last_param));
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
		send_message(PERMISSIONDENIED);
		return false;
	}
	if (command.parameters.size() < 2) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "PRIVMSG"));
		return false;
	}

    std::string param_str = command.parameters.front();

    /*  if param is channel */
    if (param_str[0] == '#')
    {
        Channel *myChannel = Channel::getChannel(param_str.substr(1));

        /*  If channel doesn't exist    */ /* Error */
        if (!myChannel)
       		send_message(ERR_NOSUCHCHANNEL(int_to_string(_id), param_str));
            return false;
        /*  Broadcast to everybody  */
        myChannel->broadcast(command.last_param + "\r\n");
    }
    /*  else param is user  */
    else
    {
        /*  If the target user doesn't exist */
        User    *myUser = User::getUser(param_str, server);         // This one is not a proper way

        if (!myUser) {
            send_message(ERR_NOSUCHNICKCHANNEL(param_str));
            return false;      
        }
    
        /*  Send the message    */
        myUser->send_message(PRIVMSG(_nick, command.last_param));
    }
    return true;
}
