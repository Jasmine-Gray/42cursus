#include "bonus/FileTransfer.hpp"

namespace
{
	const char	CTCP_DELIMITER = '\001';

	bool	isChannelTarget(const std::string &target)
	{
		return !target.empty() && target[0] == '#';
	}

	std::string	toUpper(std::string value)
	{
		for (std::string::size_type i = 0; i < value.size(); ++i)
		{
			if (value[i] >= 'a' && value[i] <= 'z')
				value[i] = static_cast<char>(value[i] - 'a' + 'A');
		}
		return value;
	}

	bool	extractCtcpPayload(const std::string &message, std::string &payload)
	{
		std::string::size_type	end;

		if (message.empty() || message[0] != CTCP_DELIMITER)
			return false;
		end = message.find(CTCP_DELIMITER, 1);
		if (end == std::string::npos)
			end = message.size();
		payload = message.substr(1, end - 1);
		return true;
	}

	std::string	nextToken(const std::string &value,
		std::string::size_type &pos)
	{
		std::string::size_type	start;

		while (pos < value.size() && value[pos] == ' ')
			++pos;
		start = pos;
		while (pos < value.size() && value[pos] != ' ')
			++pos;
		return value.substr(start, pos - start);
	}

	bool	isSupportedDccCommand(const std::string &command)
	{
		return command == "SEND" || command == "ACCEPT" || command == "RESUME";
	}
}

bool	Bonus::FileTransfer::isTransferMessage(const std::string &message)
{
	std::string				payload;
	std::string				keyword;
	std::string				command;
	std::string::size_type	pos;

	if (!extractCtcpPayload(message, payload))
		return false;
	pos = 0;
	keyword = toUpper(nextToken(payload, pos));
	command = toUpper(nextToken(payload, pos));
	return keyword == "DCC" && isSupportedDccCommand(command);
}

bool	Bonus::FileTransfer::isDirectTarget(const std::string &target)
{
	return !target.empty() && !isChannelTarget(target);
}

std::string	Bonus::FileTransfer::relayMessage(const std::string &senderNick,
	const std::string &target, const std::string &message)
{
	return ":" + senderNick + " PRIVMSG " + target + " :" + message;
}
