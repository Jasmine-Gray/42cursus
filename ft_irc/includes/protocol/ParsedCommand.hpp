#ifndef FT_IRC_PROTOCOL_PARSED_COMMAND_HPP
# define FT_IRC_PROTOCOL_PARSED_COMMAND_HPP

# include <string>
# include <vector>

class ParsedCommand
{
public:
	ParsedCommand();
	ParsedCommand(const std::string &command,
		const std::vector<std::string> &params);

	const std::string				&getCommand() const;
	const std::vector<std::string>	&getParams() const;
	bool							empty() const;

private:
	std::string					command_;
	std::vector<std::string>	params_;
};

#endif
