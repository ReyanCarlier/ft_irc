#include "Message.hpp"

Message::Message() {
    this->m_message = "";
    this->m_target = NULL;
}

Message::Message(const Message& other) {
    this->m_message = other.m_message;
    this->m_sender = other.m_sender;
    this->m_target = other.m_target;
}

Message::Message(const std::string message, Client *target) {
    this->m_message = message;
    this->m_target = target;
}

Message::~Message() {}

Message& Message::operator=(const Message& other) {
    if (this == &other)
        return *this;
    this->m_message = other.m_message;
    this->m_target = other.m_target;
    return *this;
}

const std::string Message::getMessage() const {
    return this->m_message;
}

Client  *Message::getTarget() {
    return this->m_target;
}

Client  *Message::getSender() {
    return this->m_sender;
}

void Message::setMessage(const std::string message) {
    this->m_message = message;
}

void Message::setTarget(Client *target) {
    this->m_target = target;
}

void Message::setSender(Client *sender) {
    this->m_sender = sender;
}

bool Message::send() {
    if (this->m_target == NULL)
        return false;
    // TODO: Send message to target
    return true;
}
