#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"

void	CommandDispatcher::handleNAMES(ClientSession &client,
	const Params &params)
{
	if (params.empty())
	{
		sendNamesReply(client, "*");
		return;
	}
	sendNamesReply(client, params[0]);
}
