/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frrusso <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:22:27 by frrusso           #+#    #+#             */
/*   Updated: 2023/04/27 13:22:29 by frrusso          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"

#define V_IT vector<Client*>::iterator it

class Channel {
	private:
		int						_id;
		string				_name;
		string				_topic;
		bool					_inviteOnly;
		vector<Client*>	_clients;
		vector<Client*>	_invited;
		vector<Client*>	_operators;
		vector<Client*>	_banned;
		vector<Client*>	_muted;
	public:
		/* Constructor & Destructor ***************************************** */
		Channel();
		Channel(const Channel& other);
		Channel(const string name);
		~Channel();

		/* Operator overload ************************************************ */
		Channel& operator=(const Channel& other);

		/* Getter *********************************************************** */
		int						getId() const;
		bool					isInviteOnly() const;
		string				getTopic() const;
		string				getName() const;
		vector<Client*>	getClients();
		vector<Client*>	getInvited();
		vector<Client*>	getOperators();
		vector<Client*>	getBanned();
		vector<Client*>	getMuted();

		/* Setter *********************************************************** */
		void					setId(int id);
		void					setName(string name);
		void					setTopic(string topic);
		void					setInviteOnly(bool inviteOnly);

		/* Function ********************************************************* */
		void					addClient(Client *client);
		void					addInvited(Client *client);
		void					addOperator(Client *client);
		void 					addBanned(Client *client);
		void					addMuted(Client *client);
		void					removeClient(Client *client);
		void					removeInvited(Client *client);
		void					removeOperator(Client *client);
		void					unbanClient(Client *client);
		void					unmuteClient(Client *client);
		bool					isBanned(Client *client);
		bool					isInvited(Client *client);
		bool					isMuted(Client *client);
		bool					isOperator(Client *client);
		bool					isInChannel(Client *client);
};
