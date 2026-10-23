#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

void	CommandDispatcher::handleTOPIC(ClientSession &client,
	const Params &params)
{
	if (params.empty())
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "TOPIC"));
		return;
	}
	if (!channelManager_.isMember(params[0], client.getNickname()))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOTONCHANNEL, params[0]));
		return;
	}
	if (params.size() == 1)
	{
		std::string	topic = channelManager_.getTopic(params[0]);
		if (topic.empty())
			client.enqueueReply(replyBuilder_.reply(replyTarget(client), RPL_NOTOPIC, params[0]));
		else
			client.enqueueReply(replyBuilder_.reply(replyTarget(client), RPL_TOPIC, params[0] + " :" + topic));
		return;
	}
	if (channelManager_.isTopicRestricted(params[0])
		&& !channelManager_.isOperator(params[0], client.getNickname()))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_CHANOPRIVSNEEDED, params[0]));
		return;
	}
	channelManager_.setTopic(client.getNickname(), params[0], params[1]);
	server_->broadcastToChannel(params[0], replyBuilder_.channelMessage(
		clientPrefix(client), "TOPIC", params[0] + " :" + params[1]));
}
