#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

void	CommandDispatcher::handleJOIN(ClientSession &client,
	const Params &params)
{
	std::string	key;

	if (params.empty())
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "JOIN"));
		return;
	}
	if (params.size() > 1)
		key = params[1];
	if (channelManager_.hasChannel(params[0]))
	{
		if (channelManager_.isInviteOnly(params[0])
			&& !channelManager_.isInvited(params[0], client.getNickname()))
		{
			client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_INVITEONLYCHAN, params[0]));
			return;
		}
		if (!channelManager_.hasCorrectKey(params[0], key))
		{
			client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_BADCHANNELKEY, params[0]));
			return;
		}
		if (channelManager_.isFull(params[0])
			&& !channelManager_.isMember(params[0], client.getNickname()))
		{
			client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_CHANNELISFULL, params[0]));
			return;
		}
	}
	if (!channelManager_.join(client.getNickname(), params[0], key))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOSUCHCHANNEL, params[0]));
		return;
	}
	std::string	topic = channelManager_.getTopic(params[0]);

	server_->broadcastToChannel(params[0], replyBuilder_.channelMessage(
		clientPrefix(client), "JOIN", params[0]));
	if (topic.empty())
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), RPL_NOTOPIC, params[0]));
	else
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), RPL_TOPIC, params[0] + " :" + topic));
	sendNamesReply(client, params[0]);
}
