#include "PhoneBook.hpp"
#include <iomanip>
#include <iostream>

PhoneBook::PhoneBook(void) : _contact_count(0)
{
}

PhoneBook::~PhoneBook(void)
{
}

void PhoneBook::add_contact(void)
{
	bool	is_valid;

	std::string fields[5];
	std::string labels[5] = {"First Name: ", "Last Name: ", "Nickname: ",
		"Phone Number: ", "Darkest Secret: "};
	for (int i = 0; i < 5; i++)
	{
		while (true)
		{
			std::cout << labels[i];
			if (!std::getline(std::cin, fields[i]))
				return ;
			if (fields[i].empty())
			{
				std::cout << "Error: Field cannot be empty." << std::endl;
				continue ;
			}
			if (i == 3)
			{
				is_valid = true;
				for (size_t j = 0; j < fields[i].length(); j++)
				{
					if (!std::isdigit(fields[i][j]))
					{
						is_valid = false;
						break ;
					}
				}
				if (!is_valid)
				{
					std::cout << "Error: Phone number must contain digits only." << std::endl;
					continue ;
				}
			}
			break ; 
		}
	}
	this->_contacts[this->_contact_count % 8].set_info(this->_contact_count % 8,
		fields[0], fields[1], fields[2], fields[3], fields[4]);
	this->_contact_count++;
	std::cout << "Contact added successfully!" << std::endl;
}

void PhoneBook::_print_column(std::string str) const
{
	if (str.length() > 10)
	{
		std::cout << str.substr(0, 9) << ".";
	}
	else
	{
		std::cout << std::setw(10) << str;
	}
}

void PhoneBook::search_contact(void) const
{
	if (this->_contact_count == 0)
	{
		std::cout << "Phonebook is empty." << std::endl;
		return ;
	}

	std::cout << "--------------------------------------------" << std::endl;
	std::cout << "|     Index|First Name| Last Name|  Nickname|" << std::endl;
	std::cout << "--------------------------------------------" << std::endl;

	int max = (this->_contact_count > 8) ? 8 : this->_contact_count;
	for (int i = 0; i < max; i++)
	{
		std::cout << "|";
		std::cout << std::setw(10) << i << "|";
		_print_column(this->_contacts[i].get_first_name());
		std::cout << "|";
		_print_column(this->_contacts[i].get_last_name());
		std::cout << "|";
		_print_column(this->_contacts[i].get_nickname());
		std::cout << "|" << std::endl;
	}

	std::cout << "Enter index to display details: ";
	std::string input;
	if (!(std::getline(std::cin, input)) || input.empty())
		return ;

	if (input.length() == 1 && input[0] >= '0' && input[0] < '0' + max)
	{
		int idx = input[0] - '0';
		std::cout << "First Name: " << this->_contacts[idx].get_first_name() << std::endl;
		std::cout << "Last Name: " << this->_contacts[idx].get_last_name() << std::endl;
		std::cout << "Nickname: " << this->_contacts[idx].get_nickname() << std::endl;
		std::cout << "Phone Number: " << this->_contacts[idx].get_phone_number() << std::endl;
		std::cout << "Darkest Secret: " << this->_contacts[idx].get_darkest_secret() << std::endl;
	}
	else
	{
		std::cout << "Invalid index." << std::endl;
	}
}
