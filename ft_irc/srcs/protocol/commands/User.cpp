#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"

void	CommandDispatcher::handleUSER(ClientSession &client,
	const Params &params)
{
	if (params.size() < 4)
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "USER"));
		return;
	}
	if (client.isRegistered())
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_ALREADYREGISTRED));
		return;
	}
	client.setUsername(params[0]);
	client.setRealname(params[3]);
	tryWelcome(client);
}
