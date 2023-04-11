#pragma once

#include "Client.hpp"
#include <string>
#include <iostream>
#include <list>

/**
 * Representation of a channel in the server.
 * 
 */
class Channel {

    public:
        Channel();
        Channel(const Channel& other);
        Channel(const std::string name);
        ~Channel();

        Channel& operator=(const Channel& other);

        int                 getId() const;
        std::string         getName() const;
        std::list<Client>   getClients() const;
        std::list<Client>   getOperators() const;
        std::list<Client>   getAdmins() const;
        std::list<Client>   getOwners() const;
        std::list<Client>   getBanned() const;
        std::list<Client>   getMuted() const;

        void setId(const int id);
        void setName(const std::string name);
        void addClient(const Client client);
        void addOperator(const Client client);
        void addAdmin(const Client client);
        void addOwner(const Client client);
        void addBanned(const Client client);
        void addMuted(const Client client);
        void removeClient(Client client);
        void removeOperator(Client client);
        void removeAdmin(Client client);
        void disownClient(Client client);
        void unbanClient(Client client);
        void unmuteClient(Client client);
        
    private:
        int                 _id;
        std::string         _name;
        std::list<Client>   _clients;
        std::list<Client>   _operators;
        std::list<Client>   _admins;
        std::list<Client>   _owners;
        std::list<Client>   _banned;
        std::list<Client>   _muted;
};