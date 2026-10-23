#include "protocol/CommandParser.hpp"
#include <cctype>
#include <vector>

namespace
{
	std::string	stripLineEnding(const std::string &line)
	{
		std::string	result;

		result = line;
		while (!result.empty()
			&& (result[result.size() - 1] == '\r'
				|| result[result.size() - 1] == '\n'))
			result.erase(result.size() - 1);
		return result;
	}

	void	skipSpaces(const std::string &line, std::string::size_type &pos)
	{
		while (pos < line.size() && line[pos] == ' ')
			++pos;
	}
}

ParsedCommand	CommandParser::parseLine(const std::string &line)
{
	std::string					cleanLine;
	std::string::size_type		pos;
	std::string::size_type		start;
	std::string					command;
	std::vector<std::string>	params;

	cleanLine = stripLineEnding(line);
	pos = 0;
	skipSpaces(cleanLine, pos);
	start = pos;
	while (pos < cleanLine.size() && cleanLine[pos] != ' ')
		++pos;
	command = cleanLine.substr(start, pos - start);
	if (command.empty())
		return ParsedCommand();
	command = normalizeCommand(command);
	while (pos < cleanLine.size())
	{
		skipSpaces(cleanLine, pos);
		if (pos >= cleanLine.size())
			break;
		if (cleanLine[pos] == ':')
		{
			params.push_back(cleanLine.substr(pos + 1));
			break;
		}
		start = pos;
		while (pos < cleanLine.size() && cleanLine[pos] != ' ')
			++pos;
		params.push_back(cleanLine.substr(start, pos - start));
	}
	return ParsedCommand(command, params);
}

std::string	CommandParser::normalizeCommand(const std::string &command)
{
	std::string	result;

	result = command;
	for (std::string::size_type i = 0; i < result.size(); ++i)
		result[i] = static_cast<char>(
			std::toupper(static_cast<unsigned char>(result[i])));
	return result;
}
