#include "IRC.hpp"
#include "Channel.hpp"


/*
	This bot is called Tosser

	It checks PRIVMSG and NOTICE commands.
	If the last parameter of these commands is : !tosser, the bot responds with a Head or Tail message

	Every time someones write "!toss", it will toss a coin and write the result, in the form of Head or Tail
*/


string	tosser()
{
	string message;

	srand(time(NULL));
	message = rand() % 2 ? "Head" : "Tail";
}

