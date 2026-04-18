#include "PhoneBook.hpp"
#include <iostream>
#include <iomanip> 
#include <cctype>  

PhoneBook::PhoneBook() : _currentIndex(0), _totalContacts(0)
{
}

PhoneBook::~PhoneBook()
{
}

bool PhoneBook::_hasControlChars(const std::string& str) const
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isprint(static_cast<unsigned char>(str[i])))
        {
            return true;
        }
    }
    return false; 
}

bool PhoneBook::_getInput(const std::string& prompt, std::string& input) const
{
    while (true)
    {
        std::cout << prompt;
        if (!std::getline(std::cin, input))
            return false; 

        if (input.empty())
        {
            std::cout << "Error: Field cannot be empty. Please try again." << std::endl;
            continue;
        }

        bool isOnlySpaces = true;
        for (size_t i = 0; i < input.length(); i++)
        {
            if (input[i] != ' ')
            {
                isOnlySpaces = false;
                break;
            }
        }
        if (isOnlySpaces)
        {
            std::cout << "Error: Field cannot contain only spaces. Please try again." << std::endl;
            continue;
        }

        if (_hasControlChars(input))
        {
            std::cout << "Error: Input contains invalid characters (like Tabs). Please try again." << std::endl;
            continue;
        }

        return true; 
    }
}

std::string PhoneBook::_truncate(std::string str) const
{
    if (str.length() > 10)
        return str.substr(0, 9) + ".";
    return str;
}

bool PhoneBook::_isNumeric(const std::string& str) const
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(static_cast<unsigned char>(str[i]))) // 1文字でも数字以外があれば false
            return false;
    }
    return true;
}

void PhoneBook::_addContact()
{
    std::string fName, lName, nName, pNumber, secret;

    if (!_getInput("Enter First Name: ", fName)) return;
    if (!_getInput("Enter Last Name: ", lName)) return;
    if (!_getInput("Enter Nickname: ", nName)) return;

    while (true)
    {
        if (!_getInput("Enter Phone Number: ", pNumber)) return;
        
        if (_isNumeric(pNumber))
            break; 
            
        std::cout << "Error: Phone number must contain only digits. Please try again." << std::endl;
    }

    if (!_getInput("Enter Darkest Secret: ", secret)) return;

    _contacts[_currentIndex].setInfo(fName, lName, nName, pNumber, secret);
    
    std::cout << "=> Contact successfully added at index " << _currentIndex << "!" << std::endl;

    _currentIndex = (_currentIndex + 1) % 8;
    if (_totalContacts < 8)
        _totalContacts++;
}

void PhoneBook::_searchContact() const
{
    if (_totalContacts == 0)
    {
        std::cout << "Phonebook is empty! Please ADD a contact first." << std::endl;
        return;
    }

    std::cout << std::setfill(' ') << std::right;
    
    std::cout << "|" << std::setw(10) << "Index"
              << "|" << std::setw(10) << "First Name"
              << "|" << std::setw(10) << "Last Name"
              << "|" << std::setw(10) << "Nickname" << "|" << std::endl;
    std::cout << "|----------|----------|----------|----------|" << std::endl;

    for (int i = 0; i < _totalContacts; i++)
    {
        std::cout << "|" << std::setw(10) << i
                  << "|" << std::setw(10) << _truncate(_contacts[i].getFirstName())
                  << "|" << std::setw(10) << _truncate(_contacts[i].getLastName())
                  << "|" << std::setw(10) << _truncate(_contacts[i].getNickname()) << "|" << std::endl;
    }

    std::string indexStr;
    std::cout << "Enter index to display details: ";
    if (!std::getline(std::cin, indexStr)) return; // EOF対策

    if (indexStr.length() == 1 && indexStr[0] >= '0' && indexStr[0] < '0' + _totalContacts)
    {
        int index = indexStr[0] - '0'; // 文字を整数に変換
        std::cout << "\n--- Contact Details ---" << std::endl;
        std::cout << "First Name     : " << _contacts[index].getFirstName() << std::endl;
        std::cout << "Last Name      : " << _contacts[index].getLastName() << std::endl;
        std::cout << "Nickname       : " << _contacts[index].getNickname() << std::endl;
        std::cout << "Phone Number   : " << _contacts[index].getPhoneNumber() << std::endl;
        std::cout << "Darkest Secret : " << _contacts[index].getDarkestSecret() << std::endl;
        std::cout << "-----------------------" << std::endl;
    }
    else
    {
        std::cout << "Error: Invalid index!" << std::endl;
    }
}

void PhoneBook::run()
{
    std::string command;

    std::cout << "=======================================" << std::endl;
    std::cout << "        Welcome to My PhoneBook!       " << std::endl;
    std::cout << "=======================================" << std::endl;

    while (true)
    {
        std::cout << "\nEnter command (ADD, SEARCH, EXIT): ";
        
        if (!std::getline(std::cin, command))
        {
            std::cout << "\n[EOF detected] Exiting..." << std::endl;
            break;
        }

        if (command == "EXIT")
        {
            std::cout << "Exiting PhoneBook. All contacts are lost forever!" << std::endl;
            break;
        }
        else if (command == "ADD")
        {
            this->_addContact();
        }
        else if (command == "SEARCH")
        {
            this->_searchContact();
        }
        else if (!command.empty())
        {
            std::cout << "Invalid command. Only ADD, SEARCH, and EXIT are accepted." << std::endl;
        }
    }
}
