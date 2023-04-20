# IRC Server
The objective of this project is to reproduce the operation of an IRC server. We
will use a real IRC client to connect to a server and thus test it. The Internet
operates using numerous standards and protocols to allow interoperability
between connected machines. It is always interesting to know what kind of thing.

## Authors
- [François Russo](https://www.github.com/LolinEagle)
- [Nathan Felsemberg](https://github.com/geekprod27)
- [Reyan Carlier](https://github.com/ReyanCarlier)

## Useful links
- [IRC/2 Numeric List](https://www.alien.net.au/irc/irc2numerics.html)
(RFC2812 priority / RFC1459)

## Deployment
To deploy this project run
```bash
git clone https://github.com/LolinEagle/ft_irc && cd ft_irc && make run
```

# Cheat Sheet

## PASS <password>

Example :
```
PASS passwordhere
```
Replies :
```
- ERR_NEEDMOREPARAMS (numeric)
- ERR_ALREADYREGISTRED (numeric)
```

## NICK
Parameters :
```
<password>
```
Description :
```
NICK command is used to give user a nickname or change the existing one.
```
Example :
```
NICK rclr
```
Numeric replies :
```
- ERR_NONICKNAMEGIVEN
- ERR_NICKNAMEINUSE
- ERR_UNAVAILRESOURCE
- ERR_ERRONESNICKNAME
- ERR_NICKCOLLISION
- ERR_RESTRICTED
```

__Server reply :__
```
- :bob!<...>@<hostname> rclr
	=> Server tells that bob changed his nickname to rcrlr
```

## USER

Parameters :
```
<user> <mode> <unused> <realname>
```
Description :
```
The USER command is used at the beginning of connection to specify the username,
hostname and realname of a new user.
The <realname> parameter may contain space characters.
The <mode> parameter should be a numeric, and can be used to automatically set
user modes when registering with the server.
This parameter is a bitmask, with only 2 bits having any signification.
```
Masks :
```
- bit 2 : user mode 'w' will be set
- bit 3 : user mode 'i' will be set
```

Examples :
```
- USER guest 0 * :Reyan Carlier
	=> User register itself with a username "guest" and realname "Reyan Carlier"
- User guest 8 * :Reyan Carlier
	=> User register itself with a username "guest" and realname "Reyan Carlier"
	, and asking to be invisible
```

## OPER

Parameters :
```
<name> <password>
```
Description :
```
A normal user uses the OPER command to obtain operator privileges.
The combination of <name> and <password> are required to gain Operator
privileges. Upon success, the user will receive a MODE message, indicating the
new user modes.
```
Numeric replies :
```
- ERR_NEEDMOREPARAMS
- ERR_NOOPERHOST
- RPL_YOUREOPER (success)
- ERR_PASSWDMISMATCH (incorrect password)
```
Example :
```
- OPER rcrl secretpwd
	=> Attempt to register as an operator using a username of "rcrl" and
	"secretpwd" as the password.
```

## MODE

Parameters :
```
<nickname> -/+[a | i | w | r | o | O | s]
```
Description :
```
The flag 'a' SHALL NOT be toggled by the user using the MODE command, instead
use of the AWAY command is REQUIRED. If a user attempts to make themselves an
operator using the "+o" or "+O" flag, the attempt SHOULD be ignored as users
could bypass the authentication mechanisms of the OPER command. There is no
restriction, however, on anyone `deopping' themselves (using "-o" or "-O").

On the other hand, if a user attempts to make themselves unrestricted using the
"-r" flag, the attempt SHOULD be ignored.
There is no restriction, however, on anyone `deopping' themselves (using "+r").
This flag is typically set by the server upon connection for administrative
reasons. While the restrictions imposed are left up to the implementation, it is
typical that a restricted user not be allowed to change nicknames, nor make use
of the channel operator status on channels.

The flag 's' is obsolete but MAY still be used.

The available modes are as follows:
	a - user is flagged as away;
	i - marks a users as invisible;
	w - user receives wallops;
	r - restricted user connection;
	o - operator flag;
	O - local operator flag;
	s - marks a user for receipt of server notices.
```
Numeric replies :
```
- ERR_NEEDMOREPARAMS
- ERR_UMODEUNKNOWNFLAG (incorrect flag)
- ERR_USERSDONTMATCH (user in the command isn't the user that sent it)
- RPL_UMODEIS (success)
```
Examples :
```
- MODE rcrl -w
	=> Command by rcrl to turn off reception of WALLOPS messages.
- MODE rcrl +i
	=> Command by rcrl to make itself invisible.
- MODE rcrl -o
	=> Command by rcrl to 'deop' rcrl (removing operator status).
```
