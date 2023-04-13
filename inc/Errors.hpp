#pragma once

#include <string>

struct Errors {
	static const std::string ERR_NOSUCHNICK;
	static const std::string ERR_NOSUCHCHANNEL;
	static const std::string ERR_CANNOTSENDTOCHAN;
	static const std::string ERR_NORECIPIENT;
	static const std::string ERR_NOTEXTTOSEND;
	static const std::string ERR_UNKNOWNCOMMAND;
	static const std::string ERR_NOMOTD;
	static const std::string ERR_NONICKNAMEGIVEN;
	static const std::string ERR_ERRONEUSNICKNAME;
	static const std::string ERR_NICKNAMEINUSE;
	static const std::string ERR_NOTONCHANNEL;
	static const std::string ERR_USERNOTINCHANNEL;
	static const std::string ERR_NOTREGISTERED;
	static const std::string ERR_NEEDMOREPARAMS;
	static const std::string ERR_ALREADYREGISTRED;
	static const std::string ERR_PASSWDMISMATCH;
	static const std::string ERR_YOUREBANNEDCREEP;
	static const std::string ERR_KEYSET;
	static const std::string ERR_CHANNELISFULL;
	static const std::string ERR_UNKNOWNMODE;
	static const std::string ERR_INVITEONLYCHAN;
	static const std::string ERR_BANNEDFROMCHAN;
	static const std::string ERR_BADCHANNELKEY;
	static const std::string ERR_NOPRIVILEGES;
	static const std::string ERR_CHANOPRIVSNEEDED;
	static const std::string ERR_NOOPERHOST;
	static const std::string ERR_UMODEUNKNOWNFLAG;
	static const std::string ERR_USERSDONTMATCH;
};