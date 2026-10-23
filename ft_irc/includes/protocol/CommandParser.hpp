#ifndef FT_IRC_PROTOCOL_COMMAND_PARSER_HPP
# define FT_IRC_PROTOCOL_COMMAND_PARSER_HPP

# include "protocol/ParsedCommand.hpp"
# include <string>

class CommandParser
{
public:
	static ParsedCommand	parseLine(const std::string &line);
	static std::string		normalizeCommand(const std::string &command);

private:
	CommandParser();
};

#endif
