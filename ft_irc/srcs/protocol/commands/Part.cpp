#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

void	CommandDispatcher::handlePART(ClientSession &client,
	const Params &params)
{
	std::string	channel;
	std::string	target;

	if (params.empty())
	{
		client.enqueueReply(replyBuilder_.reply(
				replyTarget(client), ERR_NEEDMOREPARAMS, "PART"));
		return;
	}
	channel = params[0];
	if (!channelManager_.hasChannel(channel))
	{
		client.enqueueReply(replyBuilder_.reply(
				replyTarget(client), ERR_NOSUCHCHANNEL, channel));
		return;
	}
	if (!channelManager_.isMember(channel, client.getNickname()))
	{
		client.enqueueReply(replyBuilder_.reply(
				replyTarget(client), ERR_NOTONCHANNEL, channel));
		return;
	}
	target = channel;
	if (params.size() > 1 && !params[1].empty())
		target += " :" + params[1];
	server_->broadcastToChannel(channel, replyBuilder_.channelMessage(
			clientPrefix(client), "PART", target));
	channelManager_.part(client.getNickname(), channel);
}
