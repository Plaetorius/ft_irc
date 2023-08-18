#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <set>
# include <vector>
# include <iostream>
# include "IRC.hpp"
# include "Channel.hpp"

using namespace std;

class User
{
	private:
		/*	User info	*/
		int		_id, _fd;
		string	_nick, _user, _name;
		
		/*	Authentication level	*/
		bool	_has_password, _has_nick, _has_user, _is_identified;
		
		/*	Pointers to the classes the user is in	*/
		vector<Channel *>	_channels;
		list<t_command>		_commands;
		t_data				*server;

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
		void	execute_commands(t_command &command);

		bool	command_PASS(t_command &command);
		bool	command_NICK(t_command &command);
		bool	command_USER(t_command &command);

		bool	command_PING(t_command &command);
		bool	command_JOIN(t_command &command);
		bool	command_names(t_command &command);
		bool	command_PRIVMSG(t_command &command);
		int		command_QUIT(t_command &command);
		bool	command_PART(t_command &command);
		bool	command_KICK(t_command &command);
		int		command_KILL(t_command &command);
		bool	command_OPER(t_command &command);
		bool	command_MODE(t_command &command);
		bool	command_TOPIC(t_command &command);
		bool	command_NOTICE(t_command &command);
		bool	command_INVITE(t_command &command);


		/*	Static function	*/
		static User *getUser(std::string nick, t_data *server);


		/*	Getters and Setters	*/
		int		get_id(void) const;
		int		get_fd(void) const;
		string	get_nick(void) const;
		bool	get_identification(void) const;
		void	set_id(int id);		//TODO remove ; for debug
		void	set_fd(int fd);		//TODO remove ; for debug
		bool    is_operator(void);
};

#endif