#ifndef PHONEBOOK_HPP
# define PHONEBOOK_HPP

# include <string>
# include "Contact.hpp"

class PhoneBook {
public:
    PhoneBook(void);
    ~PhoneBook(void);

    void    add_contact(void);
    void    search_contact(void) const;

private:
    Contact _contacts[8];
    
    int     _contact_count;

    void    _print_column(std::string str) const;
};

#endif
