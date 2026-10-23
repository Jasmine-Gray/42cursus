#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"

void	CommandDispatcher::handleCAP(ClientSession &client,
	const Params &params)
{
	if (params.empty())
	{
		client.enqueueReply(replyBuilder_.reply(
				replyTarget(client), ERR_NEEDMOREPARAMS, "CAP"));
		return;
	}
	if (params[0] == "LS" || params[0] == "LIST")
		client.enqueueReply(replyBuilder_.cap(
				replyTarget(client), params[0], ""));
	else if (params[0] == "REQ")
	{
		if (params.size() > 1)
			client.enqueueReply(replyBuilder_.cap(
					replyTarget(client), "NAK", params[1]));
		else
			client.enqueueReply(replyBuilder_.cap(
					replyTarget(client), "NAK", ""));
	}
}
