#ifndef MESSAGES_HPP
# define MESSAGES_HPP

/*  General messages    */
# define    RPL_WELCOME(nick, user, port, host) (": 001 " + nick + " :Welcome to the " + port + " Network, " + nick + "!" + user + "@" + host + "\r\n")
# define    ERR_UNKNOWNCOMMAND(client, command) (": 421 " + client + " " + command + " :Unknown command\r\n")
# define    user_id(nick, user, host) (":" + nick + "!" + user + "@" + host)


/*  ****************************************************************************** */
/*  ******************************** USER messages ******************************* */
/*  ****************************************************************************** */
# define    ERR_NEEDMOREPARAMS(client, command) ("461 " + client + " " + command + " :Not enough parameters\r\n")
# define    ERR_NOPRIVILEGES(nick) ("481 " + nick + " :Permission denied-\r\n")
# define    ERR_NOSUCHNICKCHANNEL(nick) ("401 " + nick + " : No such nick\r\n")
/*  Nick    */
# define    CHANGE_NICKNAME(old_nick, user, host, nick) (":" + old_nick + "!" + user + "@" + host + " NICK " + nick + "\r\n")
# define    ERR_NONICKNAMEGIVEN(client) (": 431 " + client + " :No nickname given\r\n")
# define    ERR_ERRONEUSNICKNAME(client, nick) (": 432 " + client + " " + nick + " :Erroneus nickname\r\n")
# define    ERR_NICKNAMEINUSE(client, nick) (": 433 " + client + " " + nick + " :Nickname is alreay in use\r\n")
/*  USER    */
# define    ERR_ALREADYREGISTERED(client) ("462 " + client + " :You may not reregister\r\n")
/*  PASS    */
# define    ERR_PASSWDMISMATCH(client) ("464 " + client + " :Password incorrect\r\n")
/*  PING    */
# define    PING(user_id, param) (user_id + " PING :" + param + "\r\n")
# define    PONG(user_id, param) (user_id + " PONG :" + param + "\r\n")
/*  Oper messages   */
# define    RPL_YOUREOPER(nick) ("381 " + nick + " :You are now an IRC operator\r\n")
/*  QUIT   */
# define    QUIT_WREASON(nick, reason) (nick + " :is exiting the channel, because " + reason + "\r\n")
# define    QUIT_WOREASON(nick) (nick + " : is exiting the channel\r\n")
/*  Kill    */
# define    KILL_WREASON(source, reason) ("You have been disconeected from the server by " + source + ", because " + reason + "\r\n")
# define    KILL_WOREASON(source) ("You have been disconeected from the server by " + source + "\r\n")
/*  ****************************************************************************** */


/*  ****************************************************************************** */
                            /*  Channel messages    */
/*  ****************************************************************************** */
# define    ERR_NOSUCHCHANNEL(nick, channel) ("; 403 " + nick + " " + channel + " :No such channel\r\n")
# define	ERR_USERNOTINCHANNEL(nick, channel) (": 441 " + nick + " " + channel + " :They aren't on that channel\r\n")
# define 	ERR_NOTONCHANNEL(channel) (": 442 " + channel + " :You're not on that channel\r\n")
# define 	ERR_USERONCHANNEL(nick, invitenick, channel) (": 443 " + nick + " " + invitenick + " " + channel + ":is already on channel\r\n")
# define    ERR_BADCHANMASK(channel) (": 476 " + channel + " :Bad Channel Mask\r\n")
/*  Join    */
# define    JOIN(nick, user, host, channel) (":" + nick + "!" + user + "@" + host + " JOIN :" + channel + "\r\n")
# define    CREATEDCHANNEL(channel) (channel + " channel created\r\n")
# define    ERR_INVITEONLYCHAN(nick, channel) (": 473 " + nick + " " + channel + " :Cannot join channel (+i)")
# define    ERR_BADCHANNELKEY(nick, channel) (": 475 " + nick + " " + channel + " :Cannot join channel (+k)\r\n")
# define    ERR_CHANNELISFULL(nick, channel) (": 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n") 
/*  Invite  */
# define	INVITE(nick, user, name, nickinvite, channel)  (":" + nick + "!" + user + "@" + name + " INVITE " + nickinvite + " " + channel + "\r\n")
# define 	RPL_INVITING(nick, user, name, invitenick, channel) ("341 " + nick + "!" + user + "@" + name + " " + invitenick + " " + channel + "\r\n")
/*  Names   */
# define    RPL_NAMREPLY(channel, nick, user, host, users) ("353 " + nick + "!" + user + "@" + host + " = " + channel + " :" + users + "\r\n")
# define    RPL_ENDOFNAMES(channel, nick, user, host) ("366 " + nick + "!" + user + "@" + host + " = " + channel + " :End of NAMES list\r\n")
/*  KICK    */

/*  PART    */
# define RPL_PART(user_id, channel, reason) (user_id + " PART #" + channel + " " + (reason.empty() ? "." : reason ) + "\r\n")
/*  Topic   */
# define	RPL_TOPIC(nick, user, name, channel, topic) (": 332 " + nick + "!" + user + "@" + name + " " + channel + " :" + topic + "\r\n")
# define	RPL_NOTOPIC(nick, user, name, channel) (": 331 " + nick + "!" + user + "@" + name + " " + channel + " :No topic is set\r\n")
/*  Modes   */
# define	ERR_CHANOPRIVSNEEDED(channel) ("482 " + channel + " :You're not channel operator\r\n")
/*  Modes   */ /*  User */
# define	MODE(nick, user, host, mode) (":" + nick + "!" + user + "@" + host + " MODE " + mode)
# define    RPL_UMODEIS(nick) ("221 " + nick + " modes are +io\r\n")
/*  Modes   */ /*  Channel */
# define    ERR_KEYSET(channel) ("467 " + channel + " :Channel key already set\r\n")
# define    RPL_CHANNELMODEIS(nick, channel) ("324 " + nick + " " + channel + " modes are +itko\r\n")



// # define    PRIVMSG(nick, user, dest, string) (":" + nick + " PRIVMSG " + " :" + str + "\r\n")
# define    PRIVMSG(nick, user, host, dest, str) (":" + nick + "!" + user + "@" + "localhost" + " PRIVMSG " + dest + " :" + str + "\r\n")
# define    PRIVMSG2(nick, user, host, dest, str) (":" + nick + "!" + user + "@" + "localhost" + " PRIVMSG " + dest + " :" + str)

#endif
