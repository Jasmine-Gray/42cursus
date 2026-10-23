#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

namespace
{
	bool	isPositiveNumber(const std::string &value)
	{
		bool	hasNonZeroDigit;

		if (value.empty())
			return false;
		hasNonZeroDigit = false;
		for (std::size_t i = 0; i < value.size(); ++i)
		{
			if (value[i] < '0' || value[i] > '9')
				return false;
			if (value[i] != '0')
				hasNonZeroDigit = true;
		}
		return hasNonZeroDigit;
	}
}

void	CommandDispatcher::handleMODE(ClientSession &client,
	const Params &params)
{
	std::string	argument;
	char		flag;

	if (params.size() < 2)
	{
		if (params.empty())
			client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "MODE"));
		else if (!channelManager_.hasChannel(params[0]))
			client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOSUCHCHANNEL, params[0]));
		else
			client.enqueueReply(replyBuilder_.reply(replyTarget(client),
				RPL_CHANNELMODEIS, params[0] + " "
				+ channelManager_.getModeString(params[0])));
		return;
	}
	if (!channelManager_.hasChannel(params[0]))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOSUCHCHANNEL, params[0]));
		return;
	}
	if (!channelManager_.isMember(params[0], client.getNickname()))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOTONCHANNEL, params[0]));
		return;
	}
	if (params[1].size() < 2
		|| (params[1][0] != '+' && params[1][0] != '-'))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_UNKNOWNMODE, params[1]));
		return;
	}
	flag = params[1][1];
	if (flag != 'i' && flag != 't' && flag != 'k'
		&& flag != 'o' && flag != 'l')
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client),
			ERR_UNKNOWNMODE, std::string(1, flag)));
		return;
	}
	if (params.size() > 2)
		argument = params[2];
	if (((flag == 'k' || flag == 'l') && params[1][0] == '+'
			&& argument.empty())
		|| (flag == 'o' && argument.empty()))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "MODE"));
		return;
	}
	if (flag == 'l' && params[1][0] == '+'
		&& !isPositiveNumber(argument))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "MODE"));
		return;
	}
	if (flag == 'o' && !channelManager_.isMember(params[0], argument))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client),
			ERR_USERNOTINCHANNEL, argument + " " + params[0]));
		return;
	}
	if (!channelManager_.setMode(client.getNickname(), params[0], params[1], argument))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_CHANOPRIVSNEEDED, params[0]));
		return;
	}
	server_->broadcastToChannel(params[0], replyBuilder_.channelMessage(
		clientPrefix(client), "MODE",
		params[0] + " " + params[1]
		+ (argument.empty() ? "" : " " + argument)));
}
