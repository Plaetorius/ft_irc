#ifndef CHANNELS_H
# define CHANNELS_H
# include "IRC.hpp"
# include <vector>

class Channel
{
	private:
		vector<int>	_fds_users;
		vector<int>	_fds_ops; //Can op users remain in the channel when turned in invite-only?
		vector<int>	_fds_invited;
		bool		_is_invite_only;
		string		_topic;
	public:
		Channel();
		Channel(const  Channel &obj);
		Channel& operator=(const Channel &rhs);
		~Channel();

		//TODO It is very likely that most functions will take the fd of the emitter of the function in order to act correctly (change topic says who did it)
		const string	get_topic(void);
		void			set_topic(string topic, int fd_emitter); 
		void			add_user(int user_fd);
		void			kick_user(int user_fd, string message);
		void			invite_user(int user_fd);
		void			uninvite_user(int user_fd);
		vector<int>		get_users(void);
		vector<int>		get_ops(void);
		vector<int>		get_invited(void);
		bool			get_invite_only(void);
		void			set_invite_only(bool mode);
};

#endif