#ifndef USER_H
# define USER_H
# include <string>
# include <iostream>
# include "IRC.hpp"

using namespace std;

class User
{
	private:
		int		_id, _fd;
		string	_nick, _user, _name;
		bool	_has_password, _has_nick, _has_user;
		bool	_is_operator, _is_identified;
		//		Something for channels;	
		
		list<t_command> _commands;

	public:
		/*	Constructors and Destructor	*/
		User();
		User(int fd, int id);
		User(const  User &obj);
		User& operator=(const User &rhs);
		~User();

		/*	Class	Methods	*/
		void	push_back_command(t_command &command);
		bool	send_message(const string &message);

		/*	command	handler	*/
		void	execute_commands();
		bool	command_PASS(t_command &command, string const& password);
		bool	command_NICK(t_command &command);
		bool	command_USER(t_command &command);
		bool	command_PING(t_command &command);
		bool	command_JOIN(t_command &command);
		bool	command_PRIVMSG(t_command &command);
		bool	command_QUIT(t_command &command);
		bool	command_PART(t_command &command);
		bool	command_KICK(t_command &command);
		int		command_KILL(t_command &command);
		bool	command_OPER(t_command &command);
		bool	command_MODE(t_command &command);
		bool	command_TOPIC(t_command &command);
		bool	command_NOTICE(t_command &command);
		bool	command_INVITE(t_command &command);

		/*	Getters and Setters	*/
		int		get_id() const;
		int		get_fd() const;
		string	get_nickname() const;
		bool	get_identification() const;
		void	set_id(int id);		//TODO remove ; for debug
		void	set_fd(int fd);		//TODO remove ; for debug
};

#endif