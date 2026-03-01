#include <iostream>
#include <string>
#include "PhoneBook.hpp"

int main(void) {
    PhoneBook phonebook;
    std::string command;

    while (true) {
        std::cout << "Enter command (ADD, SEARCH or EXIT): ";
        
        if (!std::getline(std::cin, command)) {
            break;
        }

        if (command == "ADD") {
            phonebook.add_contact();
        } else if (command == "SEARCH") {
            phonebook.search_contact();
        } else if (command == "EXIT") {
            break;
        } else if (!command.empty()) {
            std::cout << "Invalid command." << std::endl;
        }
    }
    return 0;
}
