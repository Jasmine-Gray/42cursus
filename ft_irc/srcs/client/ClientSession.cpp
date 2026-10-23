#include "client/ClientSession.hpp"

ClientSession::ClientSession(int fd)
	: fd_(fd),
	  inputBuffer_(),
	  outputQueue_(),
	  passwordAccepted_(false),
	  registered_(false),
	  disconnectRequested_(false),
	  nickname_(),
	  username_(),
	  hostname_(),
	  realname_()
{
}

int	ClientSession::getFd() const
{
	return fd_;
}

void	ClientSession::appendInput(const std::string &chunk)
{
	inputBuffer_ += chunk;
}

bool	ClientSession::popNextLine(std::string &line)
{
	std::string::size_type	pos;

	pos = inputBuffer_.find('\n');
	if (pos == std::string::npos)
		return false;
	line = inputBuffer_.substr(0, pos);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	inputBuffer_.erase(0, pos + 1);
	return true;
}

void	ClientSession::enqueueReply(const std::string &message)
{
	std::string	framed;

	framed = message;
	if (framed.size() < 2 || framed.substr(framed.size() - 2) != "\r\n")
		framed += "\r\n";
	outputQueue_.push_back(framed);
}

bool	ClientSession::hasPendingOutput() const
{
	return !outputQueue_.empty();
}

std::string	ClientSession::dequeueOutput()
{
	std::string	message;

	if (outputQueue_.empty())
		return std::string();
	message = outputQueue_.front();
	outputQueue_.pop_front();
	return message;
}

void	ClientSession::setPasswordAccepted(bool accepted)
{
	passwordAccepted_ = accepted;
	updateRegistration();
}

void	ClientSession::setNickname(const std::string &nickname)
{
	nickname_ = nickname;
	updateRegistration();
}

const std::string	&ClientSession::getNickname() const
{
	return nickname_;
}

void	ClientSession::setUsername(const std::string &username)
{
	username_ = username;
	updateRegistration();
}

const std::string	&ClientSession::getUsername() const
{
	return username_;
}

void	ClientSession::setHostname(const std::string &hostname)
{
	hostname_ = hostname;
}

const std::string	&ClientSession::getHostname() const
{
	return hostname_;
}

void	ClientSession::setRealname(const std::string &realname)
{
	realname_ = realname;
}

bool	ClientSession::isRegistered() const
{
	return registered_;
}

void	ClientSession::requestDisconnect()
{
	disconnectRequested_ = true;
}

bool	ClientSession::shouldDisconnect() const
{
	return disconnectRequested_;
}

void	ClientSession::updateRegistration()
{
	registered_ = passwordAccepted_ && !nickname_.empty() && !username_.empty();
}
