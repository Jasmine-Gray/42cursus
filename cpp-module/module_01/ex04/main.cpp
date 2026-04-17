#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void replace_in_file(const std::string& filename, const std::string& s1, const std::string& s2)
{
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open())
    {
        std::cerr << "Error: Cannot open file '" << filename << "'." << std::endl;
        return;
    }

    if (s1.empty())
    {
        std::cerr << "Error: Search string (s1) cannot be empty." << std::endl;
        ifs.close();
        return;
    }

    std::ostringstream oss;
    oss << ifs.rdbuf();
    std::string content = oss.str();
    ifs.close();

    std::string result;
    size_t pos = 0;
    size_t found_pos;

    while ((found_pos = content.find(s1, pos)) != std::string::npos)
    {
        result.append(content, pos, found_pos - pos);
        result.append(s2);
        pos = found_pos + s1.length();
    }
    result.append(content, pos, content.length() - pos);

    std::string out_filename = filename + ".replace";
    std::ofstream ofs(out_filename.c_str());
    if (!ofs.is_open())
    {
        std::cerr << "Error: Cannot create output file '" << out_filename << "'." << std::endl;
        return;
    }

    ofs << result;
    ofs.close();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./sed_is_for_losers <filename> <s1> <s2>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string s1 = argv[2];
    std::string s2 = argv[3];

    replace_in_file(filename, s1, s2);

    return 0;
}
