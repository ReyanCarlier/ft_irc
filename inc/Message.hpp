#pragma once

#include <string>
#include "Client.hpp"

/**
 * Representation of a message sent on the Server.
 * 
 * Message can either be sent from Server nor a Client.
 * 
 * Message can either be sent to the Server nor Client(s).
 */
class Message {
    public:
        Message();
        Message(const Message& other);
        Message(const std::string message, Client *target);
        ~Message();

        Message& operator=(const Message& other);

        const std::string   getMessage() const;
        Client              *getTarget();
        Client              *getSender();

        void setMessage(const std::string message);
        void setTarget(Client *target);
        void setSender(Client *sender);

        bool send();

    private:
        std::string m_message;
        Client      *m_target;
        Client      *m_sender;
};
