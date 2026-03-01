#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <string>

class Contact
{
  public:
	Contact(void);
	~Contact(void);

	void set_info(int index, std::string f_name, std::string l_name,
		std::string n_name, std::string p_number, std::string secret);
	std::string get_first_name(void) const;
	std::string get_last_name(void) const;
	std::string get_nickname(void) const;
	std::string get_phone_number(void) const;
	std::string get_darkest_secret(void) const;

  private:
	std::string _first_name;
	std::string _last_name;
	std::string _nickname;
	std::string _phone_number;
	std::string _darkest_secret;
};

#endif
