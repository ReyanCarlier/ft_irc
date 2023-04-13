#pragma once

#include "Client.hpp"
#include <string>
#include <iostream>
#include <vector>

/**
 * Representation of a channel in the server.
 */
class Channel {

    public:
        Channel();
        Channel(const Channel& other);
        Channel(const std::string name);
        ~Channel();

        Channel& operator=(const Channel& other);

        int                     getId() const;
        bool                    isInviteOnly() const;
        std::string             getTopic() const;
        std::string             getName() const;
        std::vector<Client *>   getClients();
        std::vector<Client *>   getInvited();
        std::vector<Client *>   getOperators();
        std::vector<Client *>   getAdmins();
        std::vector<Client *>   getOwners();
        std::vector<Client *>   getBanned();
        std::vector<Client *>   getMuted();

        void setId(int id);
        void setName(std::string name);
        void setTopic(std::string topic);
        void setInviteOnly(bool inviteOnly);
        void addClient(Client *client);
        void addInvited(Client *client);
        void addOperator(Client *client);
        void addAdmin(Client *client);
        void addOwner(Client *client);
        void addBanned(Client *client);
        void addMuted(Client *client);
        void removeClient(Client *client);
        void removeInvited(Client *client);
        void removeOperator(Client *client);
        void removeAdmin(Client *client);
        void disownClient(Client *client);
        void unbanClient(Client *client);
        void unmuteClient(Client *client);
        bool isBanned(Client *client);
        bool isInvited(Client *client);
        
    private:
        int                     _id;
        std::string             _name;
        std::string             _topic;
        bool                    _inviteOnly;
        std::vector<Client *>   _clients;
        std::vector<Client *>   _invited;
        std::vector<Client *>   _operators;
        std::vector<Client *>   _admins;
        std::vector<Client *>   _owners;
        std::vector<Client *>  _banned;
        std::vector<Client *>   _muted;
};
