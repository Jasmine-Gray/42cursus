#include "bonus/FileTransfer.hpp"
#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

void	CommandDispatcher::handlePRIVMSG(ClientSession &client,
	const Params &params)
{
	std::string	message;

	if (params.size() < 2)
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "PRIVMSG"));
		return;
	}
	if (Bonus::FileTransfer::isTransferMessage(params[1])
		&& Bonus::FileTransfer::isDirectTarget(params[0]))
	{
		if (!server_->sendToNick(params[0],
				Bonus::FileTransfer::relayMessage(
					clientPrefix(client), params[0], params[1])))
			client.enqueueReply(replyBuilder_.reply(
				replyTarget(client), ERR_NOSUCHNICK, params[0]));
		return;
	}
	message = ":" + clientPrefix(client) + " PRIVMSG "
		+ params[0] + " :" + params[1];
	if (!params[0].empty() && params[0][0] == '#'
		&& !channelManager_.sendToChannel(
			client.getNickname(), params[0], params[1]))
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_CANNOTSENDTOCHAN, params[0]));
		return;
	}
	if (!params[0].empty() && params[0][0] == '#')
		server_->broadcastToChannel(params[0], message, client.getFd());
	else if (!server_->sendToNick(params[0], message))
		client.enqueueReply(replyBuilder_.reply(
			replyTarget(client), ERR_NOSUCHNICK, params[0]));
}
