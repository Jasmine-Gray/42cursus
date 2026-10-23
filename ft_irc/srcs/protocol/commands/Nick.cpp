#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "protocol/IrcReplies.hpp"
#include "protocol/ReplyBuilder.hpp"
#include "server/Server.hpp"

void	CommandDispatcher::handleNICK(ClientSession &client,
	const Params &params)
{
	bool		wasRegistered;
	std::string	oldNick;

	if (params.empty() || params[0].empty())
	{
		client.enqueueReply(replyBuilder_.reply(replyTarget(client), ERR_NONICKNAMEGIVEN));
		return;
	}
	wasRegistered = client.isRegistered();
	oldNick = client.getNickname();
	if (!server_->changeNickname(client, params[0]))
	{
		client.enqueueReply(replyBuilder_.reply(
			replyTarget(client), ERR_NICKNAMEINUSE, params[0]));
		return;
	}
	if (wasRegistered)
		client.enqueueReply(":" + clientPrefix(oldNick, client)
			+ " NICK :" + params[0]);
	else
		tryWelcome(client);
}
