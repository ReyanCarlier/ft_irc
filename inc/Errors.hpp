/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:17:57 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/27 13:18:01 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <header.hpp>

struct Errors {
	static const string ERR_NOSUCHNICK;
	static const string ERR_NOSUCHCHANNEL;
	static const string ERR_CANNOTSENDTOCHAN;
	static const string ERR_NORECIPIENT;
	static const string ERR_NOTEXTTOSEND;
	static const string ERR_UNKNOWNCOMMAND;
	static const string ERR_NOMOTD;
	static const string ERR_NONICKNAMEGIVEN;
	static const string ERR_ERRONEUSNICKNAME;
	static const string ERR_NICKNAMEINUSE;
	static const string ERR_NOTONCHANNEL;
	static const string ERR_USERNOTINCHANNEL;
	static const string ERR_NOTREGISTERED;
	static const string ERR_NEEDMOREPARAMS;
	static const string ERR_ALREADYREGISTRED;
	static const string ERR_PASSWDMISMATCH;
	static const string ERR_YOUREBANNEDCREEP;
	static const string ERR_KEYSET;
	static const string ERR_CHANNELISFULL;
	static const string ERR_UNKNOWNMODE;
	static const string ERR_INVITEONLYCHAN;
	static const string ERR_BANNEDFROMCHAN;
	static const string ERR_BADCHANNELKEY;
	static const string ERR_NOPRIVILEGES;
	static const string ERR_CHANOPRIVSNEEDED;
	static const string ERR_NOOPERHOST;
	static const string ERR_UMODEUNKNOWNFLAG;
	static const string ERR_USERSDONTMATCH;
};
