#include "protocol/ReplyBuilder.hpp"
#include "protocol/IrcReplies.hpp"
#include <sstream>

namespace
{
	std::string	targetOrStar(const std::string &target)
	{
		if (target.empty())
			return "*";
		return target;
	}
}

ReplyBuilder::ReplyBuilder(const std::string &serverName)
	: serverName_(serverName)
{
}

std::string	ReplyBuilder::numeric(const std::string &target,
	const std::string &code, const std::string &message) const
{
	return ":" + serverName_ + " " + code + " " + targetOrStar(target)
		+ " " + message + "\r\n";
}

std::string	ReplyBuilder::channelMessage(const std::string &prefix,
	const std::string &command, const std::string &target) const
{
	return ":" + prefix + " " + command + " " + target + "\r\n";
}

std::string	ReplyBuilder::cap(const std::string &target,
	const std::string &subcommand, const std::string &capabilities) const
{
	return ":" + serverName_ + " CAP " + targetOrStar(target) + " "
		+ subcommand + " :" + capabilities + "\r\n";
}

std::string	ReplyBuilder::pong(const std::string &token) const
{
	if (token.empty())
		return ":" + serverName_ + " PONG " + serverName_ + "\r\n";
	return ":" + serverName_ + " PONG " + serverName_ + " :" + token + "\r\n";
}

std::string	ReplyBuilder::reply(const std::string &target, IrcReplyCode code,
	const std::string &arg) const
{
	std::ostringstream	oss;

	oss << code;
	std::string	codeStr = formatCode(code);

	for (int i = 0; REPLIES[i].code != 0; ++i)
	{
		if (REPLIES[i].code == code)
		{
			if (arg.empty())
				return numeric(target, codeStr, REPLIES[i].message);
			return numeric(target, codeStr, arg + " " + REPLIES[i].message);
		}
	}
	return "";
}

std::string	ReplyBuilder::formatCode(IrcReplyCode code) const
{
	std::ostringstream	oss;

	if (code < 10)
		oss << "00";
	else if (code < 100)
		oss << "0";
	oss << code;
	return oss.str();
}

const ReplyBuilder::NumericReply ReplyBuilder::REPLIES[] = {
	{ 1,   "RPL_WELCOME",          ":Welcome to ft_irc" },
	{ 315, "RPL_ENDOFWHO",         ":End of /WHO list" },
	{ 324, "RPL_CHANNELMODEIS",    "" },
	{ 331, "RPL_NOTOPIC",          ":No topic is set" },
	{ 332, "RPL_TOPIC",            "" },
	{ 341, "RPL_INVITING",         "" },
	{ 353, "RPL_NAMREPLY",         "" },
	{ 366, "RPL_ENDOFNAMES",       ":End of /NAMES list" },
	{ 401, "ERR_NOSUCHNICK",       ":No such nick/channel" },
	{ 403, "ERR_NOSUCHCHANNEL",    ":No such channel" },
	{ 404, "ERR_CANNOTSENDTOCHAN", ":Cannot send to channel" },
	{ 421, "ERR_UNKNOWNCOMMAND",   ":Unknown command" },
	{ 431, "ERR_NONICKNAMEGIVEN",  ":No nickname given" },
	{ 433, "ERR_NICKNAMEINUSE",    ":Nickname is already in use" },
	{ 441, "ERR_USERNOTINCHANNEL", ":They aren't on that channel" },
	{ 442, "ERR_NOTONCHANNEL",     ":You're not on that channel" },
	{ 443, "ERR_USERONCHANNEL",    ":is already on channel" },
	{ 461, "ERR_NEEDMOREPARAMS",   ":Not enough parameters" },
	{ 462, "ERR_ALREADYREGISTRED", ":You may not reregister" },
	{ 464, "ERR_PASSWDMISMATCH",   ":Password incorrect" },
	{ 471, "ERR_CHANNELISFULL",    ":Cannot join channel (+l)" },
	{ 472, "ERR_UNKNOWNMODE",      ":is unknown mode char to me" },
	{ 473, "ERR_INVITEONLYCHAN",   ":Cannot join channel (+i)" },
	{ 475, "ERR_BADCHANNELKEY",    ":Cannot join channel (+k)" },
	{ 482, "ERR_CHANOPRIVSNEEDED", ":You're not channel operator" },
	{ 0,   NULL,                   NULL }
};
