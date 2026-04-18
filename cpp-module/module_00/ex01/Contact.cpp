#include "Contact.hpp"

Contact::Contact()
{
}

Contact::~Contact()
{
}

void Contact::setInfo(const std::string& fName, const std::string& lName, 
                      const std::string& nName, const std::string& pNumber, 
                      const std::string& secret)
{
    this->_firstName = fName;
    this->_lastName = lName;
    this->_nickname = nName;
    this->_phoneNumber = pNumber;
    this->_darkestSecret = secret;
}


std::string Contact::getFirstName() const
{
    return this->_firstName;
}

std::string Contact::getLastName() const
{
    return this->_lastName;
}

std::string Contact::getNickname() const
{
    return this->_nickname;
}

std::string Contact::getPhoneNumber() const
{
    return this->_phoneNumber;
}

std::string Contact::getDarkestSecret() const
{
    return this->_darkestSecret;
}
