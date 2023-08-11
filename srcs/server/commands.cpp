#include "User.hpp"
#include "messages.hpp"
#include <sstream>
#include <ctype.h>

bool    User::send_message(const string &message)
{
    if (write(this->_fd, message.c_str(), message.length()) < 1)
		return false;

	cout << "\033[96m" << this->_id << " > " << message << "\033[0m";
	return true;
}

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
bool	User::command_PASS(t_command &command, string const& password)
{
    if (_has_password)
        send_message(ERR_ALREADYREGISTERED(int_to_string(_id)));
    else if (command.parameters.size() == 0)
        send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "PASS"));
    else if (command.parameters.front() == password)
        _has_password = true;
    else if (command.parameters.front() != password)
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
	//TODO use iterator
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
	//TODO use iterator
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
		//TODO use iterator
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

bool	User::command_PING(t_command &command)
{

}

bool	User::command_JOIN(t_command &command)
{

}

bool	User::command_PRIVMSG(t_command &command)
{

}

bool	User::command_QUIT(t_command &command)
{

}

bool	User::command_PART(t_command &command)
{

}

bool	User::command_KICK(t_command &command)
{

}

int		User::command_KILL(t_command &command)
{

}

bool	User::command_OPER(t_command &command)
{

}

bool	User::command_MODE(t_command &command)
{

}

bool	User::command_TOPIC(t_command &command)
{

}

bool	User::command_NOTICE(t_command &command)
{

}

bool	User::command_INVITE(t_command &command)
{

}
