#include "protocol/ParsedCommand.hpp"

ParsedCommand::ParsedCommand()
	: command_(),
	  params_()
{
}

ParsedCommand::ParsedCommand(const std::string &command,
	const std::vector<std::string> &params)
	: command_(command),
	  params_(params)
{
}

const std::string	&ParsedCommand::getCommand() const
{
	return command_;
}

const std::vector<std::string>	&ParsedCommand::getParams() const
{
	return params_;
}

bool	ParsedCommand::empty() const
{
	return command_.empty();
}
