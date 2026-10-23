#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"

void	CommandDispatcher::handlePASS(ClientSession &client,
	const Params &params)
{
	if (params.empty())
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NEEDMOREPARAMS, "PASS"));
		return;
	}
	if (client.isRegistered())
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_ALREADYREGISTRED));
		return;
	}
	if (params[0] != password_)
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_PASSWDMISMATCH));
		return;
	}
	client.setPasswordAccepted(true);
	tryWelcome(client);
}
