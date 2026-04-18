#ifndef CONTACT_HPP
#define CONTACT_HPP

#include <string>

class Contact
{
	private:
	    std::string _firstName;
	    std::string _lastName;
	    std::string _nickname;
	    std::string _phoneNumber;
	    std::string _darkestSecret;

	public:
	    Contact();
	    ~Contact();

	    void setInfo(const std::string& fName, const std::string& lName, 
	                 const std::string& nName, const std::string& pNumber, 
	                 const std::string& secret);

	    std::string getFirstName() const;
	    std::string getLastName() const;
	    std::string getNickname() const;
	    std::string getPhoneNumber() const;
	    std::string getDarkestSecret() const;
};

#endif
