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
 * @attention   The code doesn't support password for joining channel
 * @attention	The code don't handle TOOMANYCHANNELS, BANNEDFROMCHAN, CHANNELISFULL
 */
bool	User::command_JOIN(t_command &command)
{
	std::cout << "Salut, Je m'appelle JOIN Command" << std::endl;
	return true;
    /*  ********************************************************************* */
                                /*  Basic checks  */
    /*  ********************************************************************* */
	if (_is_identified == false) {
		send_message(PERMISSIONDENIED);
		return false;
	}
	if (command.parameters.size() == 0) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "JOIN"));
		return false;
	}

    // Check for proper name
	string	myParam = command.parameters.front();
    if (myParam[0] != '#') {
        send_message(ERR_BADCHANMASK(myParam));
        return false;
    }

    /*  After joining the channel   */
    /*
        While a client is joined to a channel, they receive all relevant information 
        about that channel including the JOIN, PART, KICK, and MODE messages affecting 
        the channel. They receive all PRIVMSG and NOTICE messages sent to the channel, 
        and they also receive QUIT messages from other clients joined to the same 
        channel (to let them know those users have left the channel and the network). 
        This allows them to keep track of other channel members and channel modes.
    */

   	/*	Check if the channel exists	*/
    t_channels::iterator it = g_data_ptr->channels.find(myParam);
    if (it != g_data_ptr->channels.end())
	{
		Channel *myChannel = g_data_ptr->channels.at(myParam); 
		if (myChannel->get_invite_only() == true)
		{
			send_message(ERR_INVITEONLYCHAN(int_to_string(_id), myParam));
			return false;
		}
		myChannel->add_user(_fd, *g_data_ptr);
		this->_channels.insert(myChannel);
	}
	else if (_is_operator == true)
	{
		Channel *newChannel = new Channel(myParam);
		g_data_ptr->channels[myParam] = newChannel;
		newChannel->add_user(_fd, *g_data_ptr);
		this->_channels.insert(newChannel);
	}
	else
	{
		send_message(ERR_NOSUCHCHANNEL(int_to_string(_id), myParam));
		return false;	
	}
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
	std::cout << "Salut, Je m'appelle TOPIC Command" << std::endl;
	return true;
	/*  ********************************************************************* */
							/*	General checks	*/
	/*  ********************************************************************* */

	/*	If not in channel	*/

		/*	ERR_NOTONCHANNEL	*/
		/*	QUIT	*/


	/*  ********************************************************************* */
							/*	Reading the topic	*/
	/*  ********************************************************************* */

	/*	If <topic>	not given	*/

		/*	if topic not set in channel	*/
			/*	RPL_NOTOPIC	*/

		/*	else	*/
			/*	RPL_TOPIC	*/
			/*	RPL_TOPICWHOTIME	*/ 
				// The info about who and at what time set the topic
		/*	QUIT	*/
	

	/*  ********************************************************************* */
							/*	Write the topic	*/
	/*  ********************************************************************* */

	/*	If protected mode and no permission	*/
		/*	ERR_CHANOPRIVSNEEDED	*/
		/*	QUIT	*/
	
	/*	If <topic> is an empty string */
		/*	Clear the channel topic	*/
		/*	QUIT	*/
	
	/*	Change the topic	*/
	/*	Broadcast message that you changed the topic	*/
}

/*  ********************************************************************* */
							/*	Names command?	*/
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
	std::cout << "Salut, Je m'appelle INVITE Command" << std::endl;
	return true;
	/*	Basic tests	*/
		/*	If not authenticated	*/
		/*	If no parameters	*/

	/*	If channel doesn't exist	*/
		/*	ERR_NOSUCHCHANNEL	*/
		/*	Exit	*/

	/*	If target user doesn't exist	*/
		/*	CUSTOM_MESSAGE	*/
		/*	EXIT	*/

	/*	If the current user not on channel	*/
		/*	ERR_NOTONCHANNEL	*/
		/*	EXIT	*/
	
	/*	If the mode is invite only and user not operator	*/
		/*	ERR_CHANOPRIVSNEEDED	*/
		/*	EXIT	*/

	/*	If user is on channel	*/
		/*	ERR_USERONCHANNEL	*/
		/*	EXIT	*/

	/*	Send RPL_INVITING to current user	*/
	/*	Send INVITE to target user	*/
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
	std::cout << "Salut, Je m'appelle PART Command" << std::endl;
	return true;

	/*  ********************************************************************* */
                                /*  Basic checks  */
    /*  ********************************************************************* */
	if (_is_identified == false) {
		send_message(PERMISSIONDENIED);
		return false;
	}
	if (command.parameters.size() == 0) {
		send_message(ERR_NEEDMOREPARAMS(int_to_string(_id), "JOIN"));
		return false;
	}

	/*	Check the channel	*/

		/*	If channel doesn't exist  */
			/*	ERR_NOSUCHCHANNEL	*/

		/*	If not on channel	*/
			/*	ERR_NOTONCHANNEL	*/

		/*	If ok	*/
			/*	Notify everybody that client quitted the channel  */

}

/**
 * @brief	request the forced removal of a user from a channel
 * 
 * @example		<channel> <user> *( "," <user> ) [<comment>]
 * 
 * @attention	One user at a time
 * @link		https://modern.ircdocs.horse/#kick-message
 */
bool	User::command_KICK(t_command &command)
{
	std::cout << "Salut, Je m'appelle KICK Command" << std::endl;
	return true;
	/*	Basic tests	*/
		/*	Not authorized	*/
		/*	Need more params	*/	// <comment> can be empty or don't exist
	
	/*	If channel doesn't exist	*/
		/*	ERR_NOSUCHCHANNEL	*/
		/*	exit  */
	
	/*	If the target user doesn't exist	*/
		/*	CUSTOM_MESSAGE	*/
		/*	exit	*/
	
	/*	If the user not in the channel	*/
		/*	ERR_NOTONCHANNEL	*/
		/*	Exit	*/
	
	/*	If no permission	*/
		/*	ERR_CHANOPRIVSNEEDED */
		/*	Exit	*/

	/*	If the target user not in Channel	*/
		/*	ERR_USERNOTINCHANNEL */
		/*	Exit	*/
	
	/*	Channel::kick_user()	*/
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
	std::cout << "Salut, Je m'appelle MODE Command" << std::endl;
	return true;

	/*	Just the values	*/
}

bool	User::command_QUIT(t_command &command)
{
	std::cout << "Salut, Je m'appelle QUIT Command" << std::endl;
	return true;

	/*	Same as deconnection	*/
}
