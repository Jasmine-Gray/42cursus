#include "channel/Channel.hpp"

Channel::Channel(const std::string &name)
	: name_(name),
	  members_(),
	  operators_(),
	  invited_(),
	  topic_(),
	  inviteOnly_(false),
	  topicRestricted_(false),
	  key_(),
	  userLimit_(0)
{
}

bool	Channel::addMember(const std::string &nickname)
{
	bool	wasEmpty;

	if (nickname.empty())
		return false;
	wasEmpty = members_.empty();
	members_.insert(nickname);
	if (wasEmpty)
		operators_.insert(nickname);
	return true;
}

bool	Channel::removeMember(const std::string &nickname)
{
	operators_.erase(nickname);
	return members_.erase(nickname) != 0;
}

bool	Channel::renameNickname(const std::string &oldNickname,
	const std::string &newNickname)
{
	bool	wasMember;
	bool	wasOperator;

	if (oldNickname.empty() || newNickname.empty() || oldNickname == newNickname)
		return false;
	wasMember = members_.erase(oldNickname) != 0;
	wasOperator = operators_.erase(oldNickname) != 0;
	if (wasMember)
		members_.insert(newNickname);
	if (wasOperator)
		operators_.insert(newNickname);
	if (invited_.erase(oldNickname) != 0)
		invited_.insert(newNickname);
	return wasMember;
}

bool	Channel::hasMember(const std::string &nickname) const
{
	return members_.find(nickname) != members_.end();
}

const std::set<std::string> &Channel::getMembers() const
{
	return members_;
}

void	Channel::addOperator(const std::string &nickname)
{
	if (hasMember(nickname))
		operators_.insert(nickname);
}

void	Channel::removeOperator(const std::string &nickname)
{
	operators_.erase(nickname);
}

bool	Channel::isOperator(const std::string &nickname) const
{
	return operators_.find(nickname) != operators_.end();
}

void	Channel::addInvite(const std::string &nickname)
{
	invited_.insert(nickname);
}

void	Channel::removeInvite(const std::string &nickname)
{
	invited_.erase(nickname);
}

bool	Channel::isInvited(const std::string &nickname) const
{
	return invited_.find(nickname) != invited_.end();
}

void	Channel::setTopic(const std::string &topic)
{
	topic_ = topic;
}

const std::string	&Channel::getTopic() const
{
	return topic_;
}

bool	Channel::isInviteOnly() const { return inviteOnly_; }
void	Channel::setInviteOnly(bool value) { inviteOnly_ = value; }

bool	Channel::isTopicRestricted() const { return topicRestricted_; }
void	Channel::setTopicRestricted(bool value) { topicRestricted_ = value; }

const std::string	&Channel::getKey() const { return key_; }
void	Channel::setKey(const std::string &key) { key_ = key; }

std::size_t	Channel::getUserLimit() const { return userLimit_; }
void	Channel::setUserLimit(std::size_t limit) { userLimit_ = limit; }

bool	Channel::isFull() const
{
	return userLimit_ != 0 && members_.size() >= userLimit_;
}
