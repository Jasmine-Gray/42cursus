#include "bonus/IrcBot.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

namespace
{
	std::string	toUpper(std::string value)
	{
		for (std::string::size_type i = 0; i < value.size(); ++i)
		{
			if (value[i] >= 'a' && value[i] <= 'z')
				value[i] = static_cast<char>(value[i] - 'a' + 'A');
		}
		return value;
	}

	std::string	trim(const std::string &value)
	{
		std::string::size_type	start;
		std::string::size_type	end;

		start = 0;
		while (start < value.size()
			&& (value[start] == ' ' || value[start] == '\t'))
			++start;
		end = value.size();
		while (end > start
			&& (value[end - 1] == ' ' || value[end - 1] == '\t'))
			--end;
		return value.substr(start, end - start);
	}

	bool	isChannelTarget(const std::string &target)
	{
		return !target.empty() && target[0] == '#';
	}

	std::string	intToString(int value)
	{
		std::ostringstream	oss;

		oss << value;
		return oss.str();
	}
}

IrcBot::IrcBot(const std::string &host, int port,
	const std::string &password, const std::string &nickname,
	const std::string &channel)
	: host_(host),
	  port_(port),
	  password_(password),
	  nickname_(nickname),
	  channel_(channel),
	  socketFd_(-1),
	  inputBuffer_(),
	  joined_(false),
	  games_()
{
	std::srand(static_cast<unsigned int>(std::time(NULL) ^ getpid()));
}

IrcBot::~IrcBot()
{
	closeSocket();
}

IrcBot::GameState::GameState()
	: active(false),
	  answer(),
	  owner(),
	  guesses(0)
{
}

int	IrcBot::run()
{
	if (!connectToServer())
		return 1;
	if (!sendLine("PASS " + password_)
		|| !sendLine("NICK " + nickname_)
		|| !sendLine("USER " + nickname_ + " 0 * :ft_irc bonus bot"))
		return 1;
	while (true)
	{
		if (!readFromServer())
			return 0;
	}
	return 0;
}

bool	IrcBot::connectToServer()
{
	struct addrinfo	hints;
	struct addrinfo	*result;
	struct addrinfo	*current;
	std::string		portString;
	int				status;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	portString = intToString(port_);
	status = getaddrinfo(host_.c_str(), portString.c_str(), &hints, &result);
	if (status != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return false;
	}
	for (current = result; current != NULL; current = current->ai_next)
	{
		socketFd_ = socket(current->ai_family, current->ai_socktype,
				current->ai_protocol);
		if (socketFd_ < 0)
			continue;
		if (connect(socketFd_, current->ai_addr, current->ai_addrlen) == 0)
		{
			freeaddrinfo(result);
			std::cout << "Bot connected as " << nickname_ << std::endl;
			return true;
		}
		closeSocket();
	}
	freeaddrinfo(result);
	std::cerr << "connect: " << std::strerror(errno) << std::endl;
	return false;
}

bool	IrcBot::sendLine(const std::string &line)
{
	std::string	wire;
	std::size_t	sentTotal;

	wire = line + "\r\n";
	sentTotal = 0;
	while (sentTotal < wire.size())
	{
		ssize_t	sent;

		sent = send(socketFd_, wire.c_str() + sentTotal,
				wire.size() - sentTotal, 0);
		if (sent > 0)
		{
			sentTotal += static_cast<std::size_t>(sent);
			continue;
		}
		const int	error = errno;

		if (sent < 0 && error == EINTR)
			continue;
		std::cerr << "send: " << std::strerror(error) << std::endl;
		return false;
	}
	return true;
}

bool	IrcBot::readFromServer()
{
	char	buffer[BOT_RECV_BUFFER_SIZE];
	ssize_t	received;

	received = recv(socketFd_, buffer, sizeof(buffer), 0);
	if (received == 0)
		return false;
	if (received < 0)
	{
		const int	error = errno;

		if (error == EINTR)
			return true;
		std::cerr << "recv: " << std::strerror(error) << std::endl;
		return false;
	}
	inputBuffer_.append(buffer, static_cast<std::size_t>(received));
	while (true)
	{
		std::string::size_type	pos;
		std::string				line;

		pos = inputBuffer_.find('\n');
		if (pos == std::string::npos)
			break;
		line = inputBuffer_.substr(0, pos);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		inputBuffer_.erase(0, pos + 1);
		handleLine(line);
	}
	return true;
}

void	IrcBot::handleLine(const std::string &line)
{
	Message	message;

	if (!parseMessage(line, message))
		return;
	if (message.command == "PING")
		handlePing(message);
	else if (message.command == "001" && !channel_.empty() && !joined_)
	{
		sendLine("JOIN " + channel_);
		joined_ = true;
	}
	else if (message.command == "PRIVMSG")
		handlePrivmsg(message);
}

void	IrcBot::handlePrivmsg(const Message &message)
{
	std::string	replyTarget;
	std::string	sender;
	std::string	context;
	std::string	reply;

	sender = extractNick(message.prefix);
	if (sender == nickname_)
		return;
	if (isChannelTarget(message.target))
	{
		replyTarget = message.target;
		context = message.target;
	}
	else
	{
		replyTarget = sender;
		context = sender;
	}
	if (replyTarget.empty())
		return;
	reply = responseFor(message.text, sender, context);
	if (reply.empty())
		return;
	sendLine("PRIVMSG " + replyTarget + " :" + reply);
}

void	IrcBot::handlePing(const Message &message)
{
	if (message.text.empty())
		sendLine("PONG");
	else
		sendLine("PONG :" + message.text);
}

bool	IrcBot::parseMessage(const std::string &line, Message &message) const
{
	std::string::size_type	pos;
	std::string::size_type	start;

	pos = 0;
	if (pos < line.size() && line[pos] == ':')
	{
		start = ++pos;
		while (pos < line.size() && line[pos] != ' ')
			++pos;
		message.prefix = line.substr(start, pos - start);
		while (pos < line.size() && line[pos] == ' ')
			++pos;
	}
	start = pos;
	while (pos < line.size() && line[pos] != ' ')
		++pos;
	message.command = toUpper(line.substr(start, pos - start));
	while (pos < line.size() && line[pos] == ' ')
		++pos;
	if (message.command == "PING")
	{
		if (pos < line.size() && line[pos] == ':')
			message.text = line.substr(pos + 1);
		else
			message.text = line.substr(pos);
		return !message.command.empty();
	}
	start = pos;
	while (pos < line.size() && line[pos] != ' ')
		++pos;
	message.target = line.substr(start, pos - start);
	while (pos < line.size() && line[pos] == ' ')
		++pos;
	if (pos < line.size() && line[pos] == ':')
		message.text = line.substr(pos + 1);
	else
		message.text = line.substr(pos);
	return !message.command.empty();
}

std::string	IrcBot::extractNick(const std::string &prefix) const
{
	std::string::size_type	end;

	end = prefix.find('!');
	if (end == std::string::npos)
		end = prefix.find('@');
	if (end == std::string::npos)
		return prefix;
	return prefix.substr(0, end);
}

std::string	IrcBot::responseFor(const std::string &text,
	const std::string &sender, const std::string &context)
{
	std::string				trimmed;
	std::string				command;
	std::string				rest;
	std::string				subCommand;
	std::string				subRest;
	std::string::size_type	space;

	trimmed = trim(text);
	if (trimmed.empty() || trimmed[0] != '!')
		return std::string();
	space = trimmed.find(' ');
	if (space == std::string::npos)
		command = trimmed;
	else
	{
		command = trimmed.substr(0, space);
		rest = trim(trimmed.substr(space + 1));
	}
	command = toUpper(command);
	if (command == "!HELP")
		return "Hit & Blow commands: !hb start, !guess <123>, !hb status, !hb giveup, !hb rules";
	if (command == "!GUESS")
		return guessHitAndBlow(rest, sender, context);
	if (command == "!HB")
	{
		space = rest.find(' ');
		if (space == std::string::npos)
			subCommand = rest;
		else
		{
			subCommand = rest.substr(0, space);
			subRest = trim(rest.substr(space + 1));
		}
		return handleHitAndBlow(toUpper(subCommand), subRest, sender, context);
	}
	return "Unknown command '" + command + "'. Try !help";
}

std::string	IrcBot::handleHitAndBlow(const std::string &command,
	const std::string &rest, const std::string &sender,
	const std::string &context)
{
	if (command.empty() || command == "RULES")
		return hitAndBlowRules();
	if (command == "START")
		return startHitAndBlow(sender, context);
	if (command == "GUESS")
		return guessHitAndBlow(rest, sender, context);
	if (command == "STATUS")
		return statusHitAndBlow(context);
	if (command == "GIVEUP" || command == "STOP")
		return giveUpHitAndBlow(sender, context);
	return "Unknown Hit & Blow command. Use !hb rules";
}

std::string	IrcBot::startHitAndBlow(const std::string &sender,
	const std::string &context)
{
	GameState	&game = games_[context];

	if (game.active)
		return "Hit & Blow is already running. Use !guess <123>.";
	game.active = true;
	game.answer = makeAnswer();
	game.owner = sender;
	game.guesses = 0;
	return "Hit & Blow started. Guess a 3-digit code with unique digits. Use !guess <123>.";
}

std::string	IrcBot::guessHitAndBlow(const std::string &guess,
	const std::string &sender, const std::string &context)
{
	std::map<std::string, GameState>::iterator	it;
	int											hits;
	int											blows;
	int											guesses;
	std::string									answer;

	it = games_.find(context);
	if (it == games_.end() || !it->second.active)
		return "No Hit & Blow game is running. Use !hb start.";
	if (!isValidGuess(guess))
		return "Invalid guess. Use 3 unique digits, like 123.";
	countHitAndBlow(it->second.answer, guess, hits, blows);
	it->second.guesses++;
	if (hits == 3)
	{
		guesses = it->second.guesses;
		answer = it->second.answer;
		games_.erase(it);
		return sender + " cleared Hit & Blow in " + intToString(guesses)
			+ " guesses. Answer was " + answer + ".";
	}
	return sender + ": " + guess + " -> " + intToString(hits)
		+ " hit, " + intToString(blows) + " blow";
}

std::string	IrcBot::statusHitAndBlow(const std::string &context) const
{
	std::map<std::string, GameState>::const_iterator	it;

	it = games_.find(context);
	if (it == games_.end() || !it->second.active)
		return "No Hit & Blow game is running. Use !hb start.";
	return "Hit & Blow is running. Owner: " + it->second.owner
		+ ", guesses: " + intToString(it->second.guesses)
		+ ". Use !guess <123>.";
}

std::string	IrcBot::giveUpHitAndBlow(const std::string &sender,
	const std::string &context)
{
	std::map<std::string, GameState>::iterator	it;
	std::string									answer;

	it = games_.find(context);
	if (it == games_.end() || !it->second.active)
		return "No Hit & Blow game is running. Use !hb start.";
	if (sender != it->second.owner)
		return "Only " + it->second.owner + " can stop this game.";
	answer = it->second.answer;
	games_.erase(it);
	return "Hit & Blow stopped. Answer was " + answer + ".";
}

std::string	IrcBot::hitAndBlowRules() const
{
	return "Hit & Blow: !hb start, then guess 3 unique digits with !guess <123>. Hit=same digit and position, Blow=same digit wrong position.";
}

std::string	IrcBot::makeAnswer() const
{
	std::string	answer;
	char		digit;

	while (answer.size() < 3)
	{
		digit = static_cast<char>('0' + (std::rand() % 10));
		if (answer.find(digit) == std::string::npos)
			answer += digit;
	}
	return answer;
}

bool	IrcBot::isValidGuess(const std::string &value) const
{
	if (value.size() != 3)
		return false;
	for (std::string::size_type i = 0; i < value.size(); ++i)
	{
		if (value[i] < '0' || value[i] > '9')
			return false;
		if (value.find(value[i], i + 1) != std::string::npos)
			return false;
	}
	return true;
}

void	IrcBot::countHitAndBlow(const std::string &answer,
	const std::string &guess, int &hits, int &blows) const
{
	hits = 0;
	blows = 0;
	for (std::string::size_type i = 0; i < guess.size(); ++i)
	{
		if (guess[i] == answer[i])
			hits++;
		else if (answer.find(guess[i]) != std::string::npos)
			blows++;
	}
}

void	IrcBot::closeSocket()
{
	if (socketFd_ >= 0)
	{
		close(socketFd_);
		socketFd_ = -1;
	}
}
