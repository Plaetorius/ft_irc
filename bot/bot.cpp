#include "IRC.hpp"
#include "Channel.hpp"

/*
	This bot is called Tosser

	Usage: ./tosser <IP> <PORT> <CHANNEL>

	Tosser will join a channel, and every time someones write "!toss",
	it will toss a coin and write the result in the channel, in the form of Head or Tail

*/

int	main(int argc, char *argv[])
{
	if (argc != 4)
		return error_str("Wrong parameters. Usage: ./tosser <IP> <PORT> <CHANNEL>"), EXIT_FAILURE;
	return EXIT_SUCCESS;
}
