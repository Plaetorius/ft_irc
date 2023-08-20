#include "IRC.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "User.hpp"

/*
	This bot is called Tosser

	It checks PRIVMSG and NOTICE commands.
	If the last parameter of these commands is : !tosser, the bot responds with a Head or Tail message

	Every time someones write "!toss", it will toss a coin and write the result, in the form of Head or Tail
*/


static string	tosser()
{
	string message;

	srand(time(NULL));
	message = rand() % 2 ? "Head" : "Tail";
	return message;
}

static bool init_bot(t_bot &bot)
{
	int		sock_opt_val = 1;
	string	auth = "PASS " + bot.password + "\r\nUSER tosser tosser localhost bot\r\nNICK tosser\r\n"; 
	char	server_message[READ_SIZE + 1];
	int		read_size;

	bot.fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (bot.fd_socket < 0)
		return error_str("socket() error");
	bot.addr_socket = sockaddr_in();
	bot.addr_socket.sin_family = AF_INET;
	bot.addr_socket.sin_port = htons(bot.port);
	bot.addr_socket.sin_addr.s_addr = inet_addr(LOCAL_HOST);
	if (connect(bot.fd_socket, (struct sockaddr *)&bot.addr_socket, sizeof(struct sockaddr_in)) < 0)
		return close(bot.fd_socket), error_str("connect() error");
	if (send(bot.fd_socket, auth.c_str(), auth.size(), 0) < 0)
		return close(bot.fd_socket), error_str("send() error when authenticating");
	bzero(server_message, READ_SIZE + 1);
	read_size = recv(bot.fd_socket, server_message, READ_SIZE, 0);
	if (read_size == -1)
		return close(bot.fd_socket), error_str("recv() error when authenticating");
	if (string(server_message, read_size).find("433") != string::npos)
		return close(bot.fd_socket), error_str("Only ONE tosser is allowed on the server");
	fcntl(bot.fd_socket, F_SETFL, O_NONBLOCK);
	return true;
}

static int	socket_read(t_bot &bot, string &server_reply, char *buff)
{

	int read_size = -1;

	bzero(buff, READ_SIZE + 1);
	read_size = recv(bot.fd_socket, buff, READ_SIZE, 0);
	server_reply.append(buff);
	return read_size;
}

static User* find_by_nick(string &nick)
{
	t_users users = g_data_ptr->users;
	t_users::iterator it = users.begin();
	t_users::iterator ite = users.end();

	for (; it != ite; it++)
		if (nick == it->second->get_nick())
			return it->second;
	return NULL;
}

static void clean_bot(t_bot &bot)
{
	close(bot.fd_socket);
}

static bool	run_bot(t_bot &bot)
{
	string	nick;
	string	server_reply;
	string	answer;
	char	server_message[READ_SIZE + 1];

	while (true)
	{
		while (socket_read(bot, server_reply, server_message) == READ_SIZE)
			;		
		if (server_reply.find("PRIVMSG") == string::npos)
			continue;
		server_reply = string(server_reply, 0, server_reply.size() - 2);
		if (server_reply.at(0) == ':')
			nick = string(server_reply, 1, server_reply.find("!") - 1);
		if (server_reply.find_last_of(":") != string::npos && server_reply.find("NICK") == string::npos)
		{
			answer = string(server_reply, server_reply.find_last_of(":") + 1, server_reply.size() - 1);
			cout << "Found nick: " << nick << endl;
		}
	}
}

int main(int argc, char *argv[])
{
	t_bot	bot;

	if (argc != 3)
		return (error_str("tosser requires 2 arguments. Usage: ./tosser <PORT> <PASSWORD>"), EXIT_FAILURE);
	if (!parsing(argv, bot.port, bot.password))
		return (EXIT_FAILURE);
	cout << "Tosser passed parsing" << endl;
	if (!init_bot(bot))
		return (EXIT_FAILURE);
	cout << "Init passed parsing" << endl;
	if (!run_bot(bot))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

