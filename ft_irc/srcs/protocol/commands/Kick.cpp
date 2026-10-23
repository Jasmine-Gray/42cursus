#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

void	CommandDispatcher::handleKICK(ClientSession &client,
	const Params &params)
{
	if (params.size() < 2)
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "KICK"));
		return;
	}
	if (!channelManager_.hasChannel(params[0]))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOSUCHCHANNEL, params[0]));
		return;
	}
	if (!channelManager_.isMember(params[0], client.getNickname()))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOTONCHANNEL, params[1]));
		return;
	}
	if (!channelManager_.isMember(params[0], params[1]))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_USERNOTINCHANNEL, params[1] + " " + params[0]));
		return;
	}
	if (!channelManager_.isOperator(params[0], client.getNickname()))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_CHANOPRIVSNEEDED, params[0]));
		return;
	}
	server_->broadcastToChannel(params[0], replyBuilder_.channelMessage(
		clientPrefix(client), "KICK", params[0] + " " + params[1]));
	channelManager_.kick(client.getNickname(), params[0], params[1]);
}
