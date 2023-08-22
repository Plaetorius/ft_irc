#include "User.hpp"

User::User(): server(g_data_ptr)
{
	cout << "Default User constructor called" << endl;
}

User::User(const User &obj): server(g_data_ptr)
{
	cout << "Copy User constructor called" << endl; //TODO
	(void)obj;
}

User& User::operator=(const User &rhs)
{
	cout << "User Assignment operator called" << endl; //TODO
	if (this == &rhs)
		return (*this);
	(void)rhs;
	return (*this);
}

User::~User() {}

User::User(int id, int fd) : _id(id), _fd(fd), _has_password(false), _has_nick(false), _has_user(false), _is_identified(false), server(g_data_ptr) {}

void	User::push_back_command(t_command &command)
{
	this->_commands.push_back(command);
}


bool    User::send_message(const string &message)
{
    if (write(this->_fd, message.c_str(), message.length()) < 1)
		return false;

	cout << "\033[96m" << this->_id << " > " << message << "\033[0m";
	return true;
}


/*	************************************************************************* */
						/*	Static functions	*/
/*	************************************************************************* */
User *User::getUser(std::string nick, t_data *server)
{
    User *myUser;

    std::vector<int>::iterator user_begin = server->open_fds.begin();
    std::vector<int>::iterator user_end = server->open_fds.end();
    while (user_begin != user_end)
    {
        myUser = server->users.at(*user_begin);
        if (nick == myUser->get_nick())
            return (myUser);
        user_begin++;
    }
    return (NULL);
}

bool    User::is_operator(void)
{
    vector<int>  operators;
    vector<int>::iterator beg;
    vector<int>::iterator end;

    operators = this->server->operator_fds; 
    beg = operators.begin();
    end = operators.end();
    while (beg != end)
    {
        if (*beg == _fd)
            return true;
        beg++;
    }
    return false;
}

bool	User::remove_channel(string channel_name)
{
    vector<Channel *>::iterator beg;
    vector<Channel *>::iterator end;

    beg = _channels.begin();
    end = _channels.end();
    while (beg != end)
    {
        if ((*beg)->get_name() == channel_name)
        {
            _channels.erase(beg);
            return true;
        }
        beg++;
    }
    return false;
}


/*
				GETTERS
*/
int	User::get_fd(void) const {return this->_fd;};
int User::get_id(void) const {return this->_id;};
string User::get_nick(void) const {return this->_nick;};
string	User::get_user(void) const {return this->_user;};
bool	User::get_identification(void) const {return this->_is_identified;};


//TODO remove ; for debug
void User::set_id(int id) {this->_id = id;};
void User::set_fd(int fd) {this->_fd = fd;};