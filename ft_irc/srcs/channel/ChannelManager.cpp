#include "channel/ChannelManager.hpp"
#include <sstream>

ChannelManager::ChannelManager()
	: channels_()
{
}

bool	ChannelManager::join(const std::string &nickname,
	const std::string &channelName, const std::string &key)
{
	Channel	*channel;

	if (!isChannelName(channelName))
		return false;
	channel = &getOrCreateChannel(channelName);
	if (channel->isInviteOnly() && !channel->isInvited(nickname))
		return false;
	if (!channel->getKey().empty() && channel->getKey() != key)
		return false;
	if (channel->isFull() && !channel->hasMember(nickname))
		return false;
	channel->removeInvite(nickname);
	return channel->addMember(nickname);
}

bool	ChannelManager::part(const std::string &nickname,
	const std::string &channelName)
{
	Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL)
		return false;
	return channel->removeMember(nickname);
}

bool	ChannelManager::sendToChannel(const std::string &fromNickname,
	const std::string &channelName, const std::string &message)
{
	const Channel	*channel;

	(void)message;
	channel = findChannel(channelName);
	if (channel == NULL)
		return false;
	return channel->hasMember(fromNickname);
}

bool	ChannelManager::kick(const std::string &actorNickname,
	const std::string &channelName, const std::string &targetNickname)
{
	Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL || !channel->isOperator(actorNickname))
		return false;
	return channel->removeMember(targetNickname);
}

bool	ChannelManager::invite(const std::string &actorNickname,
	const std::string &channelName, const std::string &targetNickname)
{
	Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL || !channel->isOperator(actorNickname))
		return false;
	channel->addInvite(targetNickname);
	return true;
}

bool	ChannelManager::setTopic(const std::string &actorNickname,
	const std::string &channelName, const std::string &topic)
{
	Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL)
		return false;
	if (channel->isTopicRestricted() && !channel->isOperator(actorNickname))
		return false;
	channel->setTopic(topic);
	return true;
}

bool	ChannelManager::setMode(const std::string &actorNickname,
	const std::string &channelName, const std::string &mode,
	const std::string &argument)
{
	Channel		*channel;
	bool		adding;
	char		flag;

	channel = findChannel(channelName);
	if (channel == NULL || !channel->isOperator(actorNickname))
		return false;
	if (mode.size() < 2)
		return false;
	adding = (mode[0] == '+');
	flag = mode[1];
	if (flag == 'i')
		channel->setInviteOnly(adding);
	else if (flag == 't')
		channel->setTopicRestricted(adding);
	else if (flag == 'k')
		channel->setKey(adding ? argument : "");
	else if (flag == 'o')
	{
		if (argument.empty())
			return false;
		if (adding)
			channel->addOperator(argument);
		else
			channel->removeOperator(argument);
	}
	else if (flag == 'l')
	{
		if (adding)
		{
			std::istringstream	iss(argument);
			std::size_t			limit;
			iss >> limit;
			channel->setUserLimit(limit);
		}
		else
			channel->setUserLimit(0);
	}
	return true;
}

void	ChannelManager::renameNickname(const std::string &oldNickname,
	const std::string &newNickname)
{
	for (ChannelMap::iterator it = channels_.begin();
		it != channels_.end(); ++it)
		it->second.renameNickname(oldNickname, newNickname);
}

void	ChannelManager::removeNickname(const std::string &nickname)
{
	for (ChannelMap::iterator it = channels_.begin();
		it != channels_.end(); ++it)
	{
		it->second.removeMember(nickname);
		it->second.removeInvite(nickname);
	}
}

std::vector<std::string>	ChannelManager::getChannelsForNick(
	const std::string &nickname) const
{
	std::vector<std::string>	channels;

	for (ChannelMap::const_iterator it = channels_.begin();
		it != channels_.end(); ++it)
	{
		if (it->second.hasMember(nickname))
			channels.push_back(it->first);
	}
	return channels;
}

bool	ChannelManager::isMember(const std::string &channelName,
	const std::string &nickname) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL)
		return false;
	return channel->hasMember(nickname);
}

std::string	ChannelManager::getTopic(const std::string &channelName) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL)
		return "";
	return channel->getTopic();
}

std::set<std::string>	ChannelManager::getMembers(const std::string &channelName) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL)
		return std::set<std::string>();
	return channel->getMembers();
}

bool	ChannelManager::isOperator(const std::string &channelName,
	const std::string &nickname) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	if (channel == NULL)
		return false;
	return channel->isOperator(nickname);
}

bool	ChannelManager::isInviteOnly(const std::string &channelName) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	return channel != NULL && channel->isInviteOnly();
}

bool	ChannelManager::isInvited(const std::string &channelName,
	const std::string &nickname) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	return channel != NULL && channel->isInvited(nickname);
}

bool	ChannelManager::hasCorrectKey(const std::string &channelName,
	const std::string &key) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	return channel == NULL || channel->getKey().empty()
		|| channel->getKey() == key;
}

bool	ChannelManager::isFull(const std::string &channelName) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	return channel != NULL && channel->isFull();
}

bool	ChannelManager::isTopicRestricted(const std::string &channelName) const
{
	const Channel	*channel;

	channel = findChannel(channelName);
	return channel != NULL && channel->isTopicRestricted();
}

std::string	ChannelManager::getModeString(const std::string &channelName) const
{
	const Channel	*channel;
	std::string		modes;

	channel = findChannel(channelName);
	if (channel == NULL)
		return "";
	modes = "+";
	if (channel->isInviteOnly())
		modes += "i";
	if (channel->isTopicRestricted())
		modes += "t";
	if (!channel->getKey().empty())
		modes += "k";
	if (channel->getUserLimit() != 0)
		modes += "l";
	return modes;
}

bool	ChannelManager::hasChannel(const std::string & channelName) const
{
	return findChannel(channelName) != NULL;
}

Channel	&ChannelManager::getOrCreateChannel(const std::string &channelName)
{
	ChannelMap::iterator	found;

	found = channels_.find(channelName);
	if (found == channels_.end())
		found = channels_.insert(ChannelMap::value_type(
					channelName, Channel(channelName))).first;
	return found->second;
}

const Channel	*ChannelManager::findChannel(
	const std::string &channelName) const
{
	ChannelMap::const_iterator	found;

	found = channels_.find(channelName);
	if (found == channels_.end())
		return NULL;
	return &found->second;
}

Channel	*ChannelManager::findChannel(const std::string &channelName)
{
	ChannelMap::iterator	found;

	found = channels_.find(channelName);
	if (found == channels_.end())
		return NULL;
	return &found->second;
}

bool	ChannelManager::isChannelName(const std::string &channelName) const
{
	return !channelName.empty() && channelName[0] == '#';
}
