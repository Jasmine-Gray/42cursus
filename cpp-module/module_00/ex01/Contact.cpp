#include "Contact.hpp"

Contact::Contact(void) {
}

Contact::~Contact(void) {
}

void Contact::set_info(int index, std::string f_name, std::string l_name, std::string n_name, std::string p_number, std::string secret) {
    (void)index;
    this->_first_name = f_name;
    this->_last_name = l_name;
    this->_nickname = n_name;
    this->_phone_number = p_number;
    this->_darkest_secret = secret;
}

std::string Contact::get_first_name(void) const {
    return (this->_first_name);
}

std::string Contact::get_last_name(void) const {
    return (this->_last_name);
}

std::string Contact::get_nickname(void) const {
    return (this->_nickname);
}

std::string Contact::get_phone_number(void) const {
    return (this->_phone_number);
}

std::string Contact::get_darkest_secret(void) const {
    return (this->_darkest_secret);
}
