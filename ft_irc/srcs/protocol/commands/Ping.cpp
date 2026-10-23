#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"

void	CommandDispatcher::handlePING(ClientSession &client,
	const Params &params)
{
	if (params.empty())
	{
		client.enqueueReply(replyBuilder_.reply(
			replyTarget(client), ERR_NEEDMOREPARAMS, "PING"));
		return;
	}
	client.enqueueReply(replyBuilder_.pong(params[0]));
}

void	CommandDispatcher::handlePONG(ClientSession &client,
	const Params &params)
{
	(void)client;
	(void)params;
}
