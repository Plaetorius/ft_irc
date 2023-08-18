#ifndef CHANNELS_H
# define CHANNELS_H
# include "IRC.hpp"
# include <vector>

class Channel
{
	private:
		string			_name;
		vector<int>		_fds_users;
		vector<int>		_fds_ops;
		vector<int>		_fds_invited;
		bool			_is_invite_only;
		string			_topic;
		bool			_is_topic_set;
		bool			_is_topic_protected;
		string			_key;
		bool			_is_channel_locked;
		unsigned int	_max_users;
		bool			_has_user_limit;
		bool			error_feedback(int fd_emitter, string message);
	public:
		Channel();
		Channel(string name, int fd_creator, t_data &data);
		~Channel();

		bool			is_user(int	fd_user);
		bool			is_op(int fd_user);
		bool			is_invited(int fd_user);
		
		bool			add_user(int fd_user, t_data &data);
		bool			add_user(int fd_user, string &key, t_data &data);
		bool			kick_user(int emitter_fd, int to_kick_fd, string message, t_data &data);
		bool			part(int fd_user, string message, t_data &data);
		bool			op_user(int fd_emitter, int fd_to_op, t_data &data);
		bool			deop_user(int fd_emitter, int fd_to_deop, t_data &data);
		bool			invite_user(int fd_emitter, int fd_to_invite, t_data &data);
		bool			enable_locked_mode(int fd_emitter, string &key, t_data &data);
		bool			disable_locked_mode(int fd_emitter, t_data &data);

		bool			set_invite_only(bool mode, int fd_emitter);
		bool			set_topic(string topic, int fd_emitter, t_data &data);
		bool			set_protected_topic(bool mode, int fd_emitter, t_data &data);
		bool			set_max_users(int max_users, int fd_emitter, t_data &data);
		bool			set_has_user_limit(bool mode, int fd_emitter, t_data &data);


		string			get_name(void) const;
		vector<int>		get_users(void) const;
		vector<int>		get_ops(void) const;
		vector<int>		get_invited(void) const;
		bool			get_invite_only(void) const;
		string			get_topic(void) const;
		bool			get_topic_set(void) const;
		bool			get_topic_protected(void) const;
		string			get_key(void) const;
		bool			get_channel_locked(void) const;
		unsigned int	get_user_limit(void) const;
		bool			get_has_user_limit(void) const;

		void			broadcast(string message);
};

#endif