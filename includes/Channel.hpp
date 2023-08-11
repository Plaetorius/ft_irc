#ifndef CHANNELS_H
# define CHANNELS_H
# include "IRC.hpp"
# include <vector>

class Channel
{
	private:
		string		_name;
		vector<int>	_fds_users;
		vector<int>	_fds_ops; //when invite mode is turned on, add all the current users to the invited list
		vector<int>	_fds_invited;
		bool		_is_invite_only;
		string		_topic;
		bool		is_user(int	fd_user);
		bool		is_op(int fd_user);
		bool		is_invited(int fd_user);
		bool		error_feedback(int fd_emitter, string message);
	public:
		Channel();
		Channel(const  Channel &obj);
		Channel& operator=(const Channel &rhs);
		~Channel();

		//TODO It is very likely that most functions will take the fd of the emitter of the function in order to act correctly (change topic says who did it)
		//TODO currently we display users by their fds but change it to user ID
		
		bool			add_user(int fd_user, t_data &data);
		bool			kick_user(int emitter_fd, int to_kick_fd, string message, t_data &data);
		bool			op_user(int fd_emitter, int fd_to_op, t_data &data);
		bool			deop_user(int fd_emitter, int fd_to_deop, t_data &data);
		bool			invite_user(int fd_emitter, int fd_to_invite, t_data &data);
		bool			uninvite_user(int fd_emitter, int fd_to_uninvite, t_data &data);
		void			set_invite_only(bool mode, t_data &data);
		void			set_topic(string topic, int fd_emitter, t_data &data); //TODO maybe change to reference

		const string	get_name(void) const;
		vector<int>		get_users(void);
		vector<int>		get_ops(void);
		vector<int>		get_invited(void);
		bool			get_invite_only(void);
		const string	get_topic(void) const;

		void			broadcast(string message, int fd_emitter);
};

#endif