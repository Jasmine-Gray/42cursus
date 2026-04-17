#include <iostream>
#include <string>

int main()
{
    std::string str = "HI THIS IS BRAIN";
    
    std::string* stringPTR = &str;
    std::string& stringREF = str;

    std::cout << "--- Memory Addresses ---" << std::endl;
    std::cout << "Address of str:       " << &str << std::endl;
    std::cout << "Address by stringPTR: " << stringPTR << std::endl;
    std::cout << "Address by stringREF: " << &stringREF << std::endl;

    std::cout << "\n--- Values ---" << std::endl;
    std::cout << "Value of str:       " << str << std::endl;
    std::cout << "Value by stringPTR: " << *stringPTR << std::endl;
    std::cout << "Value by stringREF: " << stringREF << std::endl;

    return 0;
}
