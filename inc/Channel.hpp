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

        int                 getId() const;
        std::string         getName() const;
        std::vector<Client> getClients() const;
        std::vector<Client> getOperators() const;
        std::vector<Client> getAdmins() const;
        std::vector<Client> getOwners() const;
        std::vector<Client> getBanned() const;
        std::vector<Client> getMuted() const;

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
        std::vector<Client>   _clients;
        std::vector<Client>   _operators;
        std::vector<Client>   _admins;
        std::vector<Client>   _owners;
        std::vector<Client>   _banned;
        std::vector<Client>   _muted;
};
