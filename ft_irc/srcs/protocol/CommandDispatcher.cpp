#include "protocol/CommandDispatcher.hpp"
#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"
#include <set>
#include <sstream>

CommandDispatcher::CommandDispatcher(ChannelManager &channelManager,
	const ReplyBuilder &replyBuilder, const std::string &password,
	Server *server)
	: channelManager_(channelManager),
	  replyBuilder_(replyBuilder),
	  password_(password),
	  server_(server)
{
}

void	CommandDispatcher::dispatch(ClientSession &client,
	const ParsedCommand &command)
{
	const std::string	&name = command.getCommand();
	const Params		&params = command.getParams();

	if (command.empty())
		return;
	if (name == "PASS")
		handlePASS(client, params);
	else if (name == "NICK")
		handleNICK(client, params);
	else if (name == "USER")
		handleUSER(client, params);
	else if (name == "CAP")
		handleCAP(client, params);
	else if (name == "PING")
		handlePING(client, params);
	else if (name == "PONG")
		handlePONG(client, params);
	else if (name == "QUIT")
		handleQUIT(client, params);
	else if (name == "JOIN")
		handleJOIN(client, params);
	else if (name == "PART")
		handlePART(client, params);
	else if (name == "NAMES")
		handleNAMES(client, params);
	else if (name == "WHO")
		handleWHO(client, params);
	else if (name == "PRIVMSG")
		handlePRIVMSG(client, params);
	else if (name == "KICK")
		handleKICK(client, params);
	else if (name == "INVITE")
		handleINVITE(client, params);
	else if (name == "TOPIC")
		handleTOPIC(client, params);
	else if (name == "MODE")
		handleMODE(client, params);
	else
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_UNKNOWNCOMMAND, name));
}

void	CommandDispatcher::sendNamesReply(ClientSession &client,
	const std::string &channelName) const
{
	std::set<std::string>			members;
	std::ostringstream				nameList;
	std::ostringstream				arg;
	std::set<std::string>::iterator	it;

	if (channelManager_.hasChannel(channelName))
	{
		members = channelManager_.getMembers(channelName);
		for (it = members.begin(); it != members.end(); ++it)
		{
			if (it != members.begin())
				nameList << " ";
			if (channelManager_.isOperator(channelName, *it))
				nameList << "@";
			nameList << *it;
		}
		arg << "= " << channelName << " :" << nameList.str();
		client.enqueueReply(replyBuilder_.reply(
				replyTarget(client), RPL_NAMREPLY, arg.str()));
	}
	client.enqueueReply(replyBuilder_.reply(
			replyTarget(client), RPL_ENDOFNAMES, channelName));
}

void	CommandDispatcher::tryWelcome(ClientSession &client)
{
	if (client.isRegistered())
		client.enqueueReply(replyBuilder_.reply(client.getNickname(), RPL_WELCOME));
}

std::string	CommandDispatcher::replyTarget(const ClientSession &client) const
{
	if (client.getNickname().empty())
		return "*";
	return client.getNickname();
}

std::string	CommandDispatcher::clientPrefix(const ClientSession &client) const
{
	return clientPrefix(client.getNickname(), client);
}

std::string	CommandDispatcher::clientPrefix(const std::string &nickname,
	const ClientSession &client) const
{
	std::string	prefix;

	prefix = nickname;
	if (!client.getUsername().empty())
		prefix += "!" + client.getUsername();
	if (!client.getHostname().empty())
		prefix += "@" + client.getHostname();
	return prefix;
}
