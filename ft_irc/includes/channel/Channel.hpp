#ifndef FT_IRC_CHANNEL_CHANNEL_HPP
# define FT_IRC_CHANNEL_CHANNEL_HPP

# include <cstddef>
# include <set>
# include <string>

class Channel
{
public:
	explicit Channel(const std::string &name);

	bool				addMember(const std::string &nickname);
	bool				removeMember(const std::string &nickname);
	bool				renameNickname(const std::string &oldNickname,
							const std::string &newNickname);
	bool				hasMember(const std::string &nickname) const;
	const std::set<std::string> &getMembers() const;
	void				addOperator(const std::string &nickname);
	void				removeOperator(const std::string &nickname);
	bool				isOperator(const std::string &nickname) const;
	void				addInvite(const std::string &nickname);
	void				removeInvite(const std::string &nickname);
	bool				isInvited(const std::string &nickname) const;
	void				setTopic(const std::string &topic);
	const std::string	&getTopic() const;

	// mode
	bool				isInviteOnly() const;
	void				setInviteOnly(bool value);
	bool				isTopicRestricted() const;
	void				setTopicRestricted(bool value);
	const std::string	&getKey() const;
	void				setKey(const std::string &key);
	std::size_t			getUserLimit() const;
	void				setUserLimit(std::size_t limit);
	bool				isFull() const;

private:
	std::string				name_;
	std::set<std::string>	members_;
	std::set<std::string>	operators_;
	std::set<std::string>	invited_;
	std::string				topic_;
	bool					inviteOnly_;
	bool					topicRestricted_;
	std::string				key_;
	std::size_t				userLimit_;
};

#endif
