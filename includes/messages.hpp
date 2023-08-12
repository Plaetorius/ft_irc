#ifndef MESSAGES_HPP
# define MESSAGES_HPP

/*  General messages    */
# define    RPL_WELCOME(client, nick, user, port) (client + " :Welcome to the " + port + " Network, " + nick + "[!" + user + "@" + LOCAL_HOST + "]/r/n")
# define    PASSWORD "je_suis_coder_en_java"

/*  PASS messages    */
# define    ERR_ALREADYREGISTERED(client) ("462 :" + client + " :You may not reregister\r\n")
# define    ERR_NEEDMOREPARAMS(client, command) ("461 :" + client + " " + command + " :Not enough parametes\r\n")
# define    ERR_PASSWDMISMATCH(client) ("464 :" + client + " :Password incorrect/r/n")

/*  NICK messages    */
# define    ERR_NONICKNAMEGIVEN(client) ("431 :" + client + " :No nickname given/r/n")
# define    ERR_ERRONEUSNICKNAME(client, nick) ("432 :" + client + " " + nick + " :Erroneus nickname\r\n")
# define    ERR_NICKNAMEINUSE(client, nick) ("433 :" + client + " " + nick + " :Nickname is alreay in use\r\n")
# define    CHANGE_NICKNAME(old_nick, nick) (":" + old_nick + " NICK " + nick + "\r\n")
# define    PERMISSIONDENIED "ERROR :You must provide a valid password using the PASS command before accessing any function/r/n"

/*  Oper messages   */
# define    RPL_YOUREOPER(client) (client + " :You are now an IRC operator")

/*  Channel messages    */
# define    ERR_BADCHANMASK(channel) ("476 :" + channel + " :Bad Channel Mask")
# define    ERR_INVITEONLYCHAN(client, channel) ("473 :" + client + " " + channel + " :Cannot join channel (+i)")
# define    ERR_NOSUCHCHANNEL(client, channel) ("403 :" + client + " " + channel + " :No such channel")

#endif
