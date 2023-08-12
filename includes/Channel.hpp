#ifndef CHANNELS_H
# define CHANNELS_H
# include "IRC.hpp"
# include <vector>

class Channel
{
	private:
		string		_name;
		vector<int>	_fds_users;
		vector<int>	_fds_ops;
		vector<int>	_fds_invited;
		bool		_is_invite_only;
		string		_topic;
		bool		is_user(int	fd_user);
		bool		is_op(int fd_user);
		bool		is_invited(int fd_user);
		bool		error_feedback(int fd_emitter, string message);
		void		add_users_to_invited(void);
	public:
		Channel();
		Channel(const std::string name);
		Channel(const  Channel &obj);
		Channel& operator=(const Channel &rhs);
		~Channel();
		
		bool			add_user(int fd_user, t_data &data);
		bool			kick_user(int emitter_fd, int to_kick_fd, string message, t_data &data);
		bool			op_user(int fd_emitter, int fd_to_op, t_data &data);
		bool			invite_user(int fd_emitter, int fd_to_invite, t_data &data);
	
		bool			set_invite_only(bool mode, int fd_emitter);
		bool			set_topic(string topic, int fd_emitter, t_data &data); //LATER maybe change to reference

		string			get_name(void) const;
		vector<int>		get_users(void) const;
		vector<int>		get_ops(void) const;
		vector<int>		get_invited(void) const;
		bool			get_invite_only(void) const;
		string			get_topic(void) const;

		void			broadcast(string message, int fd_emitter);
};

#endif