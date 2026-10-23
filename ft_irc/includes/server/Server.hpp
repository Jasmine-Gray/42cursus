#ifndef FT_IRC_SERVER_SERVER_HPP
# define FT_IRC_SERVER_SERVER_HPP

# define RECV_BUFFER_SIZE 4096
# define POLL_TIMEOUT_MS 100

# include "channel/ChannelManager.hpp"
# include "client/ClientSession.hpp"
# include "protocol/CommandDispatcher.hpp"
# include "protocol/ReplyBuilder.hpp"
# include <map>
# include <string>
# include <vector>
# include <fcntl.h>
# include <netinet/in.h>
# include <poll.h>
# include <sys/socket.h>
# include <unistd.h>

class Server
{
public:
	Server(int port, const std::string &password);
	~Server();

	int		run();
	void	sendToClient(int fd, const std::string &message);
	bool	sendToNick(const std::string &nick, const std::string &message);
	void	broadcastToChannel(const std::string &channel,
			const std::string &message, int excludeFd = -1);
	bool	isNickInUse(const std::string &nick) const;
	bool	changeNickname(ClientSession &client, const std::string &newNick);

private:
	typedef std::map<int, ClientSession>	ClientMap;
	typedef std::map<int, std::string>	OutputMap;

	bool	setupListenSocket();
	bool	setNonBlocking(int fd) const;
	void	closeListenSocket();
	void	updatePollEvents();
	void	acceptClients();
	void	handleClientRead(int fd);
	void	handleClientWrite(int fd);
	void	closeClient(int fd);
	void	queuePendingReplies(int fd);
	void	removePollFd(int fd);

	int								port_;
	std::string						password_;
	int								serverFd_;
	std::vector<struct pollfd>		pollfds_;
	ClientMap						clients_;
	OutputMap						outputBuffers_;
	ChannelManager					channelManager_;
	ReplyBuilder					replyBuilder_;
	CommandDispatcher				dispatcher_;
	std::map<std::string, int>		nickToFd_;
};

#endif
