#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"

void	CommandDispatcher::handleWHO(ClientSession &client,
	const Params &params)
{
	std::string	target;

	if (params.empty())
		target = "*";
	else
		target = params[0];
	client.enqueueReply(replyBuilder_.reply(
			replyTarget(client), RPL_ENDOFWHO, target));
}
