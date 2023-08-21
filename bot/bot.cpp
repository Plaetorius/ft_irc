#include "IRC.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "User.hpp"

/*
	This bot is called Tosser

	It checks PRIVMSG and NOTICE commands.
	If the last parameter of these commands is : ?tosser, the bot responds with a Head or Tail message

	Every time someones write "!toss", it will toss a coin and write the result, in the form of Head or Tail
	Do not put the "#" before the channel name for the parameter channel (3)
*/

t_bot	*g_bot_ptr;


static string	tosser()
{
	string message;

	srand(time(NULL));
	message = rand() % 2 ? "Head" : "Tail";
	return message;
}

static bool init_bot(t_bot &bot)
{
	string	auth = "PASS " + bot.password + "\r\nUSER tosser tosser localhost bot\r\nNICK tosser\r\nJOIN " + bot.channel + "\r\n"; 
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
	fcntl(bot.fd_socket, F_SETFL, O_NONBLOCK);
	if (string(server_message, read_size).find("433") != string::npos)
		return close(bot.fd_socket), error_str("Only ONE tosser is allowed on the server. You shall die.");
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

static bool	run_bot(t_bot &bot)
{
	char	server_message[READ_SIZE + 1];
	int		read_size;
	size_t	pos;

	while (true)
	{
		sleep(1);
		do read_size = socket_read(bot, bot.server_reply, server_message); while (read_size == READ_SIZE);
		if (bot.server_reply.find("PRIVMSG") == string::npos)
			continue;
		pos = bot.server_reply.find_last_of(":");
		if (pos != string::npos && bot.server_reply.find("NICK") == string::npos)
		{
			bot.server_reply = bot.server_reply.substr(pos + 1);
			if (bot.server_reply.find("?tosser") != string::npos)
			{
				cout << "?tosser command " << endl;
				bot.result = "PRIVMSG " + bot.channel + " :" + tosser();
				send(bot.fd_socket, bot.result.c_str(), bot.result.size(), 0);
			}
			else if (bot.server_reply.find("irc") != string::npos)
			{
				cout << "Master command" << endl;
				bot.result = "PRIVMSG "  + bot.channel + " :akalimol & tgernez are my masters.";
				send(bot.fd_socket, bot.result.c_str(), bot.result.size(), 0);
			}
			else if (bot.server_reply.find("quoi") != string::npos ||  bot.server_reply.find("Quoi") != string::npos ||  bot.server_reply.find("QUOI") != string::npos)
			{
				cout << "feur command" << endl;
				bot.result = "PRIVMSG "  + bot.channel + " :feur";
				send(bot.fd_socket, bot.result.c_str(), bot.result.size(), 0);	
			}
			else if (bot.server_reply.find("transcendence") != string::npos)
			{
				cout << "feur command" << endl;
				bot.result = "PRIVMSG "  + bot.channel + " :transcendenceRIZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
				send(bot.fd_socket, bot.result.c_str(), bot.result.size(), 0);	
			}
		}
		bot.result.clear();
		bot.server_reply.clear();
	}
}

void	signal_handler(int code)
{
	(void)code;
	sleep(1);
	close(g_bot_ptr->fd_socket);
	g_bot_ptr->result.~string();
	g_bot_ptr->server_reply.~string();
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	t_bot				bot;
	struct sigaction	sa;

	if (argc != 4)
		return (error_str("tosser requires 3 arguments. Usage: ./tosser <PORT> <PASSWORD> <CHANNEL>"), EXIT_FAILURE);
	if (!parsing(argv, bot.port, bot.password))
		return (EXIT_FAILURE);
	bot.channel = argv[3];
	if (bot.channel.empty())
		return (error_str("Wrong channel name (must start without the # and not be empty)"), EXIT_FAILURE);
	bot.channel = "#" + bot.channel;
	if (!init_bot(bot))
		return (EXIT_FAILURE);
	g_bot_ptr = &bot;  
	sa.sa_handler = signal_handler;
	sa.sa_flags = SA_RESTART; 					//Set to 0 if causes problems with syscalls
	sigfillset(&sa.sa_mask);					//Blocks the other signals when the handler is called
	if (sigaction(SIGINT, &sa, NULL) < 0)
		return (close(bot.fd_socket), error_str("sigaction() error"), EXIT_FAILURE);
	cout << "Connected to channel " + bot.channel << " and listenning" << endl;
	if (!run_bot(bot))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

