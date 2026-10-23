#ifndef FT_IRC_PROTOCOL_REPLY_BUILDER_HPP
# define FT_IRC_PROTOCOL_REPLY_BUILDER_HPP

# include "protocol/IrcReplies.hpp"
# include <string>

class ReplyBuilder
{
public:
	explicit ReplyBuilder(const std::string &serverName);

	std::string	numeric(const std::string &target, const std::string &code,
		const std::string &message) const;
	std::string	channelMessage(const std::string &prefix,
		const std::string &command, const std::string &target) const;
	std::string	cap(const std::string &target,
		const std::string &subcommand, const std::string &capabilities) const;
	std::string	pong(const std::string &token) const;
	std::string	reply(const std::string &target, IrcReplyCode code,
		const std::string &arg = "") const;

private:
	struct NumericReply {
		int			code;
		const char	*name;
		const char	*message;
	};
	std::string	formatCode(IrcReplyCode code) const;
	static const NumericReply	REPLIES[];
	std::string					serverName_;
};

#endif
