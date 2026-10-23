#include "channel/ChannelManager.hpp"
#include "client/ClientSession.hpp"
#include "protocol/CommandDispatcher.hpp"
#include "server/Server.hpp"
#include <set>
#include <vector>

void	CommandDispatcher::handleQUIT(ClientSession &client,
	const Params &params)
{
	std::vector<std::string>				channels;
	std::vector<std::string>::iterator	channelIt;
	std::set<std::string>				notifiedMembers;
	std::set<std::string>				members;
	std::set<std::string>::iterator		memberIt;
	std::string							nickName;
	std::string							message;
	std::string							reason;

	reason = "Client Quit";
	if (!params.empty())
		reason = params[0];
	nickName = client.getNickname();
	if (!nickName.empty())
	{
		message = ":" + clientPrefix(client) + " QUIT :" + reason;
		channels = channelManager_.getChannelsForNick(nickName);
		for (channelIt = channels.begin(); channelIt != channels.end();
			++channelIt)
		{
			members = channelManager_.getMembers(*channelIt);
			for (memberIt = members.begin(); memberIt != members.end();
				++memberIt)
			{
				if (*memberIt != nickName
					&& notifiedMembers.insert(*memberIt).second)
					server_->sendToNick(*memberIt, message);
			}
		}
	}
	client.requestDisconnect();
}
