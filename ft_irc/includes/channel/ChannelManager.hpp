#ifndef FT_IRC_CHANNEL_CHANNEL_MANAGER_HPP
# define FT_IRC_CHANNEL_CHANNEL_MANAGER_HPP

# include "channel/Channel.hpp"
# include <cstddef>
# include <map>
# include <string>
# include <vector>

class ChannelManager
{
public:
	ChannelManager();

	bool		join(const std::string &nickname, const std::string &channelName,
		const std::string &key);
	bool		part(const std::string &nickname, const std::string &channelName);
	bool		sendToChannel(const std::string &fromNickname,
		const std::string &channelName, const std::string &message);
	bool		kick(const std::string &actorNickname,
		const std::string &channelName,
		const std::string &targetNickname);
	bool		invite(const std::string &actorNickname,
		const std::string &channelName,
		const std::string &targetNickname);
	bool		setTopic(const std::string &actorNickname,
		const std::string &channelName,
		const std::string &topic);
	bool		setMode(const std::string &actorNickname,
		const std::string &channelName,
		const std::string &mode, const std::string &argument);
	void		renameNickname(const std::string &oldNickname,
		const std::string &newNickname);
	void		removeNickname(const std::string &nickname);
	std::vector<std::string>	getChannelsForNick(
		const std::string &nickname) const;
	bool		isMember(const std::string &channelName,
		const std::string &nickname) const;
	std::string	getTopic(const std::string &channelName) const;
	std::set<std::string>	getMembers(const std::string &channelName) const;
	bool		isOperator(const std::string &channelName,
		const std::string &nickname) const;
	bool		isInviteOnly(const std::string &channelName) const;
	bool		isInvited(const std::string &channelName,
		const std::string &nickname) const;
	bool		hasCorrectKey(const std::string &channelName,
		const std::string &key) const;
	bool		isFull(const std::string &channelName) const;
	bool		isTopicRestricted(const std::string &channelName) const;
	std::string	getModeString(const std::string &channelName) const;
	bool		hasChannel(const std::string &channelName) const;

private:
	typedef std::map<std::string, Channel>	ChannelMap;

	Channel		&getOrCreateChannel(const std::string &channelName);
	const Channel	*findChannel(const std::string &channelName) const;
	Channel			*findChannel(const std::string &channelName);
	bool			isChannelName(const std::string &channelName) const;

	ChannelMap	channels_;
};

#endif
