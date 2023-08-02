#ifndef IRC_HPP
# define IRC_HPP
# include <iostream>
# include <string>
# include <map>
# include <list>
# include <vector>
# include <algorithm>
# include <errno.h>

using namespace std;

# define LOCAL_HOST "127.0.0.1"


/******************************************************************************/
/*																		      */
/*                                  Parsing                                   */
/*																		      */
/******************************************************************************/

bool	parsing(char **argv, int &port, string &password);

/******************************************************************************/
/*																		      */
/*									Error									  */
/*																		      */
/******************************************************************************/

void	error_str(const string error_message);

/******************************************************************************/
/*																		      */
/*									Init									  */
/*																		      */
/******************************************************************************/

bool	init(int &port, std::string &password);


#endif