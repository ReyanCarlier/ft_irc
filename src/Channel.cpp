#include "Channel.hpp"

Channel::Channel() {
    this->_id = 0;
    this->_name = "";
    this->_topic = "No topic yet";
    this->_inviteOnly = false;
}

Channel::Channel(const Channel& other) {
    this->_id = other._id;
    this->_name = other._name;
    this->_clients = other._clients;
    this->_operators = other._operators;
    this->_admins = other._admins;
    this->_owners = other._owners;
    this->_banned = other._banned;
    this->_muted = other._muted;
    this->_topic = other._topic;
    this->_inviteOnly = other._inviteOnly;
}

Channel::Channel(const std::string name) {
    this->_id = 0;
    this->_name = name;
    this->_topic = "No topic yet";
    this->_inviteOnly = false;
    std::cout << "Channel " << this->_name << " created." << std::endl;
}

Channel::~Channel() { std::cout << "Channel " << this->_name << " deleted." << std::endl;}

Channel& Channel::operator=(const Channel& other) {
    if (this == &other)
        return *this;
    this->_id = other._id;
    this->_name = other._name;
    this->_clients = other._clients;
    this->_operators = other._operators;
    this->_admins = other._admins;
    this->_owners = other._owners;
    this->_banned = other._banned;
    this->_muted = other._muted;
    return *this;
}

/**
 * Returns the unique identified of the Channel.
*/
int Channel::getId() const {
    return this->_id;
}

/**
 * Returns the name of the Channel.
*/
std::string Channel::getName() const {
    return this->_name;
}

/**
 * Returns the list of Client that are in the Channel.
*/
std::vector<Client *> Channel::getClients() {
    return this->_clients;
}

/**
 * Returns the list of Client that are in the Channel with operator privileges.
*/
std::vector<Client *> Channel::getOperators() {
    return this->_operators;
}

/**
 * Returns the list of Client that are in the Channel with admin privileges.
*/
std::vector<Client *> Channel::getAdmins() {
    return this->_admins;
}

/**
 * Returns the list of Client that are in the Channel with ownership.
*/
std::vector<Client *> Channel::getOwners() {
    return this->_owners;
}

/**
 * Returns the list of Client that are banned from the Channel.
 * > They can't join the channel anymore.
*/
std::vector<Client *> Channel::getBanned() {
    return this->_banned;
}

/**
 * Returns the list of Client that are muted in the Channel.
 * > They can't send messages to the channel anymore.
*/
std::vector<Client *> Channel::getMuted() {
    return this->_muted;
}

/**
 * Sets the unique identifier of the channel.
*/
void Channel::setId(const int id) {
    this->_id = id;
}

/**
 * @brief Sets the name of a Channel.
 * 
 * @param name 
 */
void Channel::setName(const std::string name) {
    this->_name = name;
}

/**
 * @brief Adds a Client to the _clients list of a Channel.
 * When a message is sent to a Channel, each Client receives it.
 * Also, the list of Client of a Channel is this list.
 * 
 * @param client 
 */
void Channel::addClient(Client *client) {
    this->_clients.push_back(client);
}

void Channel::addOperator(Client *client) {
    this->_operators.push_back(client);
}

void Channel::addAdmin(Client *client) {
    this->_admins.push_back(client);
}

void Channel::addOwner(Client *client) {
    this->_owners.push_back(client);
}

/**
 * @brief Adds a Client to the _banned list of the Channel.
 * A banned Client cannot join the Channel.
 * 
 * @param client 
 */
void Channel::addBanned(Client *client) {
    this->_banned.push_back(client);
}

/**
 * @brief Adds a Client to the _muted list of the Channel.
 * A muted Client cannot send messages to the Channel.
 * 
 * @param client 
 */
void Channel::addMuted(Client *client) {
    this->_muted.push_back(client);
}

/**
 * @brief Removes a Client from a Channel.
 * 
 * @param client 
 */
void Channel::removeClient(Client *client) {
    for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if ((*it)->getSocket() == client->getSocket()) {
			_clients.erase(it);
			break;
		}
	}
}

void Channel::removeOperator(Client *client) {
    for (std::vector<Client *>::iterator it = this->_operators.begin(); it != this->_operators.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            this->_operators.erase(it);
            break;
        }
    }
}

void Channel::removeAdmin(Client *client) {
    for (std::vector<Client *>::iterator it = this->_admins.begin(); it != this->_admins.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            this->_admins.erase(it);
            break;
        }
    }
}

void Channel::disownClient(Client *client) {
    for (std::vector<Client *>::iterator it = this->_owners.begin(); it != this->_owners.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            this->_owners.erase(it);
            break;
        }
    }
}

/**
 * @brief Removes a Client from the _banned list.
 * The Client is now able to re-join the Channel.
 * 
 * @param client 
 */
void Channel::unbanClient(Client *client) {
    for (std::vector<Client *>::iterator it = this->_banned.begin(); it != this->_banned.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            this->_banned.erase(it);
            break;
        }
    }
}

/**
 * @brief Removes a Client from the _muted list.
 * The Client is now able to re-speak in the Channel.
 * 
 * @param client 
 */
void Channel::unmuteClient(Client *client) {
    for (std::vector<Client *>::iterator it = this->_muted.begin(); it != this->_muted.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            this->_muted.erase(it);
            break;
        }
    }
}

bool Channel::isBanned(Client *client) {
    for (std::vector<Client *>::iterator it = this->_banned.begin(); it != this->_banned.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            return true;
        }
    }
    return false;
}

bool Channel::isMuted(Client *client) {
    for (std::vector<Client *>::iterator it = this->_muted.begin(); it != this->_muted.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            return true;
        }
    }
    return false;
}

bool Channel::isOperator(Client *client) {
    for (std::vector<Client *>::iterator it = this->_operators.begin(); it != this->_operators.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            return true;
        }
    }
    return false;
}

// INVITATION //
bool Channel::isInviteOnly() const {
    return this->_inviteOnly;
}

void Channel::setInviteOnly(bool inviteOnly) {
    this->_inviteOnly = inviteOnly;
}

void Channel::addInvited(Client *client) {
    this->_invited.push_back(client);
}

void Channel::removeInvited(Client *client) {
    for (std::vector<Client *>::iterator it = this->_invited.begin(); it != this->_invited.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            this->_invited.erase(it);
            break;
        }
    }
}

bool Channel::isInvited(Client *client) {
    for (std::vector<Client *>::iterator it = this->_invited.begin(); it != this->_invited.end(); it++) {
        if ((*it)->getSocket() == client->getSocket()) {
            return true;
        }
    }
    return false;
}

// TOPIC //
/**
 * @brief Sets the topic of a Channel.
 * 
 * @param topic 
 */
void Channel::setTopic(const std::string topic) {
    this->_topic = topic;
}

/**
 * @brief Returns the topic of a Channel.
 * 
 * @return std::string 
 */
std::string Channel::getTopic() const {
    return this->_topic;
}