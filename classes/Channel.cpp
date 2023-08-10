#include "Channel.hpp"
#include <iostream>

Channel::Channel()
{
	std::cout << "Default Channel constructor called" << std::endl;
}

Channel::Channel(const Channel &obj)
{
	std::cout << "Copy Channel constructor called" << std::endl;
}

Channel& Channel::operator=(const Channel &rhs)
{
	std::cout << "Channel Assignment operator called" << std::endl;
	if (this == &rhs)
		return (*this);
}

Channel::~Channel()
{
	std::cout << "Channel destructor called" << std::endl;
}