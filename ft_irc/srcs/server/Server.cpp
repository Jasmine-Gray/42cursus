#include "server/Server.hpp"
#include "protocol/CommandParser.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <set>
#include <vector>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

namespace
{
	volatile sig_atomic_t	g_stopRequested = 0;

	void	handleStopSignal(int signalNumber)
	{
		(void)signalNumber;
		g_stopRequested = 1;
	}

	bool	installStopSignalHandler(int signalNumber)
	{
		struct sigaction	action;

		std::memset(&action, 0, sizeof(action));
		action.sa_handler = handleStopSignal;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		return sigaction(signalNumber, &action, NULL) == 0;
	}

	std::string	formatPeerHost(const struct sockaddr_in &address)
	{
		const char	*host;

		host = inet_ntoa(address.sin_addr);
		if (host == NULL)
			return "0.0.0.0";
		return host;
	}
}

Server::Server(int port, const std::string &password)
	: port_(port),
	  password_(password),
	  serverFd_(-1),
	  pollfds_(),
	  clients_(),
	  outputBuffers_(),
	  channelManager_(),
	  replyBuilder_("ft_irc"),
	  dispatcher_(channelManager_, replyBuilder_, password_, this),
	  nickToFd_()
{
}

Server::~Server()
{
	while (!clients_.empty())
		closeClient(clients_.begin()->first);
	closeListenSocket();
}

int	Server::run()
{
	const int	pollTimeoutMs = POLL_TIMEOUT_MS;

	g_stopRequested = 0;
	signal(SIGPIPE, SIG_IGN);
	if (!installStopSignalHandler(SIGINT)
		|| !installStopSignalHandler(SIGTERM))
	{
		std::cerr << "sigaction: " << std::strerror(errno) << std::endl;
		return 1;
	}
	if (!setupListenSocket())
		return 1;
	std::cout << "Server is listening on port " << port_ << std::endl;
	while (!g_stopRequested)
	{
		int	pollResult;

		updatePollEvents();
		pollResult = poll(&pollfds_[0], pollfds_.size(), pollTimeoutMs);
		if (pollResult < 0)
		{
			const int	error = errno;

			if (error == EINTR)
				continue;
			std::cerr << "poll:" << std::strerror(error) << std::endl;
			return 1;
		}
		if (pollResult == 0)
			continue;
		for (std::size_t i = 0; i < pollfds_.size();)
		{
			const int	fd = pollfds_[i].fd;
			const short	revents = pollfds_[i].revents;

			if (revents == 0)
			{
				i++;
				continue;
			}
			if (fd == serverFd_)
			{
				if (revents & POLLIN)
					acceptClients();
				i++;
				continue;
			}
			if (revents & POLLIN)
				handleClientRead(fd);
			if (clients_.find(fd) != clients_.end() && (revents & POLLOUT))
				handleClientWrite(fd);
			if (clients_.find(fd) != clients_.end() && (revents & (POLLERR | POLLHUP | POLLNVAL)))
				closeClient(fd);
			if (i < pollfds_.size() && pollfds_[i].fd == fd)
				i++;
		}
	}
	return 0;
}

void	Server::sendToClient(int fd, const std::string &message)
{
	ClientMap::iterator	clientIt;

	clientIt = clients_.find(fd);
	if (clientIt != clients_.end())
		clientIt->second.enqueueReply(message);
	else
		std::cerr << "Warning: Attempted to send to non-existent FD "
			<< fd << std::endl;
}

bool	Server::sendToNick(const std::string &nick, const std::string &message)
{
	std::map<std::string, int>::iterator	nickIt;

	nickIt = nickToFd_.find(nick);
	if (nickIt == nickToFd_.end())
		return false;
	sendToClient(nickIt->second, message);
	return true;
}

void	Server::broadcastToChannel(const std::string &channel,
	const std::string &message, int excludeFd)
{
	std::set<std::string>				members;
	std::set<std::string>::iterator		it;
	std::map<std::string, int>::iterator	nickIt;

	members = channelManager_.getMembers(channel);
	for (it = members.begin(); it != members.end(); ++it)
	{
		nickIt = nickToFd_.find(*it);
		if (nickIt != nickToFd_.end() && nickIt->second != excludeFd)
			sendToClient(nickIt->second, message);
	}
}

bool	Server::isNickInUse(const std::string &nick) const
{
	return nickToFd_.find(nick) != nickToFd_.end();
}

bool	Server::changeNickname(ClientSession &client, const std::string &newNick)
{
	std::string oldNick;

	if (isNickInUse(newNick))
		return false;

	oldNick = client.getNickname();
	if (!oldNick.empty())
	{
		nickToFd_.erase(oldNick);
		channelManager_.renameNickname(oldNick, newNick);
	}

	nickToFd_[newNick] = client.getFd();
	client.setNickname(newNick);
	return true;
}

bool	Server::setupListenSocket()
{
	sockaddr_in address;
	pollfd		listenPollFd;

	serverFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd_ < 0)
	{
		std::cerr << "socket: " << std::strerror(errno) << std::endl;
		return false;
	}
	if (!setNonBlocking(serverFd_))
	{
		closeListenSocket();
		return false;
	}
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(static_cast<unsigned short>(port_));
	if (bind(serverFd_, reinterpret_cast<sockaddr *>(&address),
			sizeof(address)) < 0)
	{
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		closeListenSocket();
		return false;
	}
	if (listen(serverFd_, SOMAXCONN) < 0)
	{
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		closeListenSocket();
		return false;
	}
	listenPollFd.fd = serverFd_;
	listenPollFd.events = POLLIN;
	listenPollFd.revents = 0;
	pollfds_.push_back(listenPollFd);
	return true;
}

bool	Server::setNonBlocking(int fd) const
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		const int	error = errno;

		std::cerr << "fcntl: " << std::strerror(error) << std::endl;
		return false;
	}
	return true;
}

void	Server::closeListenSocket()
{
	if (serverFd_ >= 0)
	{
		close(serverFd_);
		removePollFd(serverFd_);
		serverFd_ = -1;
	}
}

void	Server::updatePollEvents()
{
	for (std::size_t i = 0; i < pollfds_.size(); ++i)
	{
		ClientMap::iterator	client;
		OutputMap::iterator	output;

		pollfds_[i].revents = 0;
		if (pollfds_[i].fd == serverFd_)
		{
			pollfds_[i].events = POLLIN;
			continue;
		}
		client = clients_.find(pollfds_[i].fd);
		if (client == clients_.end())
			continue;
		output = outputBuffers_.find(pollfds_[i].fd);
		pollfds_[i].events = POLLIN;
		if ((output != outputBuffers_.end() && !output->second.empty())
			|| client->second.hasPendingOutput())
			pollfds_[i].events |= POLLOUT;
	}
}

void	Server::acceptClients()
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen;
	int					clientFd;
	struct pollfd		clientPollfd;

	clientLen = sizeof(clientAddr);
	clientFd = accept(serverFd_,
		reinterpret_cast<struct sockaddr *>(&clientAddr), &clientLen);
	if (clientFd < 0)
	{
		const int	error = errno;

		if (error == EAGAIN || error == EWOULDBLOCK || error == EINTR)
			return;
		std::cerr << "accept: " << std::strerror(error) << std::endl;
		return;
	}
	if (!setNonBlocking(clientFd))
	{
		close(clientFd);
		return;
	}
	clientPollfd.fd = clientFd;
	clientPollfd.events = POLLIN;
	clientPollfd.revents = 0;
	pollfds_.push_back(clientPollfd);
	ClientSession	client(clientFd);
	client.setHostname(formatPeerHost(clientAddr));
	clients_.insert(ClientMap::value_type(clientFd, client));
	outputBuffers_[clientFd] = std::string();
}

void	Server::handleClientRead(int fd)
{
	char				buffer[RECV_BUFFER_SIZE];
	ssize_t				received;
	ClientMap::iterator	client;
	std::string			line;

	received = recv(fd, buffer, sizeof(buffer), 0);
	if (received > 0)
	{
		client = clients_.find(fd);
		if (client == clients_.end())
			return;
		client->second.appendInput(std::string(buffer,
				static_cast<std::size_t>(received)));
		while (clients_.find(fd) != clients_.end()
			&& client->second.popNextLine(line))
		{
			if (!line.empty())
				dispatcher_.dispatch(client->second,
					CommandParser::parseLine(line));
			queuePendingReplies(fd);
			client = clients_.find(fd);
			if (client == clients_.end())
				return;
			if (client->second.shouldDisconnect())
			{
				closeClient(fd);
				return;
			}
		}
		return;
	}
	if (received == 0)
	{
		closeClient(fd);
		return;
	}
	const int	error = errno;

	if (error == EAGAIN || error == EWOULDBLOCK || error == EINTR)
		return;
	std::cerr << "recv: " << std::strerror(error) << std::endl;
	closeClient(fd);
}

void	Server::handleClientWrite(int fd)
{
	OutputMap::iterator	output;
	ssize_t				sent;

	queuePendingReplies(fd);
	output = outputBuffers_.find(fd);
	if (output == outputBuffers_.end() || output->second.empty())
		return;
	sent = send(fd, output->second.c_str(), output->second.size(), 0);
	if (sent > 0)
	{
		output->second.erase(0, static_cast<std::size_t>(sent));
		return;
	}
	if (sent == 0)
		return;

	const int	error = errno;

	if (error == EAGAIN || error == EWOULDBLOCK || error == EINTR)
		return;
	std::cerr << "send: " << std::strerror(error) << std::endl;
	closeClient(fd);
}

void	Server::closeClient(int fd)
{
	ClientMap::iterator	clientIt;
	std::string			nickName;

	clientIt = clients_.find(fd);
	if (clientIt != clients_.end())
	{
		nickName = clientIt->second.getNickname();
		if (!nickName.empty())
		{
			nickToFd_.erase(nickName);
			channelManager_.removeNickname(nickName);
		}
		clients_.erase(clientIt);
	}
	close(fd);
	outputBuffers_.erase(fd);
	removePollFd(fd);
}

void	Server::queuePendingReplies(int fd)
{
	ClientMap::iterator	found;

	found = clients_.find(fd);
	if (found == clients_.end())
		return;
	while (found->second.hasPendingOutput())
		outputBuffers_[fd] += found->second.dequeueOutput();
}

void	Server::removePollFd(int fd)
{
	for (std::vector<struct pollfd>::iterator it = pollfds_.begin();
		it != pollfds_.end(); ++it)
	{
		if (it->fd == fd)
		{
			pollfds_.erase(it);
			return;
		}
	}
}
