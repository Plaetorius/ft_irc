#include "IRC.hpp"
#include "Channel.hpp"

/*
	This bot is called Tosser

	Usage: ./tosser <PORT> <PASSWORD> <CHANNEL>

	Tosser will join a channel, and every time someones write "!toss",
	it will toss a coin and write the result in the channel, in the form of Head or Tail

*/

int	main(int argc, char *argv[])
{
	string	message;
	int		fd_bot;
	Channel	*channel;
	int		port;
	string	password;

	if (argc != 4)
		return error_str("Wrong parameters. Usage: ./tosser <PORT> <PASSWORD> <CHANNEL>"), EXIT_FAILURE;
	if (parsing(argv, port, password) == false)
		return EXIT_FAILURE;
	srand(time(NULL));
	message = rand() % 2 ? "Head" : "Tail";
	channel = g_data_ptr->channels.at(argv[3]);
	
	return EXIT_SUCCESS;
}
