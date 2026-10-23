#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

void	CommandDispatcher::handleINVITE(ClientSession &client,
	const Params &params)
{
	if (params.size() < 2)
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "INVITE"));
		return;
	}
	if (!channelManager_.isMember(params[1], client.getNickname()))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOTONCHANNEL, params[1]));
		return;
	}
	if (channelManager_.isMember(params[1], params[0]))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_USERONCHANNEL, params[0] + " " + params[1]));
		return;
	}
	if (!server_->isNickInUse(params[0]))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NOSUCHNICK, params[0]));
		return;
	}
	if (!channelManager_.invite(client.getNickname(), params[1], params[0]))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_CHANOPRIVSNEEDED, params[1]));
		return;
	}
	if (!server_->sendToNick(params[0], ":" + clientPrefix(client)
		+ " INVITE " + params[0] + " " + params[1]))
	{
		client.enqueueReply(replyBuilder_.reply(
			replyTarget(client), ERR_NOSUCHNICK, params[0]));
		return;
	}
	client.enqueueReply(replyBuilder_.reply(replyTarget(client), RPL_INVITING, params[1] + " " + params[0]));
}
