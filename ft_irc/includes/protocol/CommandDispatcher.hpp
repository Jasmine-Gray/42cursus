#ifndef FT_IRC_PROTOCOL_COMMAND_DISPATCHER_HPP
# define FT_IRC_PROTOCOL_COMMAND_DISPATCHER_HPP

# include "protocol/ParsedCommand.hpp"
# include <string>
# include <vector>

class ChannelManager;
class ClientSession;
class ReplyBuilder;
class Server;

class CommandDispatcher
{
public:
	CommandDispatcher(ChannelManager &channelManager,
		const ReplyBuilder &replyBuilder, const std::string &password,
		Server *server);

	void	dispatch(ClientSession &client, const ParsedCommand &command);

private:
	typedef std::vector<std::string>	Params;

	void		handlePASS(ClientSession &client, const Params &params);
	void		handleNICK(ClientSession &client, const Params &params);
	void		handleUSER(ClientSession &client, const Params &params);
	void		handleCAP(ClientSession &client, const Params &params);
	void		handlePING(ClientSession &client, const Params &params);
	void		handlePONG(ClientSession &client, const Params &params);
	void		handleQUIT(ClientSession &client, const Params &params);
	void		handleJOIN(ClientSession &client, const Params &params);
	void		handlePART(ClientSession &client, const Params &params);
	void		handleNAMES(ClientSession &client, const Params &params);
	void		handleWHO(ClientSession &client, const Params &params);
	void		handlePRIVMSG(ClientSession &client, const Params &params);
	void		handleKICK(ClientSession &client, const Params &params);
	void		handleINVITE(ClientSession &client, const Params &params);
	void		handleTOPIC(ClientSession &client, const Params &params);
	void		handleMODE(ClientSession &client, const Params &params);
	void		sendNamesReply(ClientSession &client,
					const std::string &channelName) const;
	void		tryWelcome(ClientSession &client);
	std::string	replyTarget(const ClientSession &client) const;
	std::string	clientPrefix(const ClientSession &client) const;
	std::string	clientPrefix(const std::string &nickname,
					const ClientSession &client) const;

	ChannelManager		&channelManager_;
	const ReplyBuilder	&replyBuilder_;
	std::string			password_;
	Server				*server_;
};

#endif
