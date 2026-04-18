#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include "Contact.hpp"

class PhoneBook
{
private:
    Contact _contacts[8];
    
    int _currentIndex;
    int _totalContacts;

    bool _getInput(const std::string& prompt, std::string& input) const;
    
    std::string _truncate(std::string str) const;
    bool _isNumeric(const std::string& str) const;
    bool _hasControlChars(const std::string& str) const;

    void _addContact();
    void _searchContact() const;

public:
    PhoneBook();
    ~PhoneBook();

    void run();
};

#endif
