#include "bonus/IrcBot.hpp"

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <signal.h>

namespace
{
	bool	parsePort(const char *value, int &port)
	{
		char	*end;
		long	parsed;

		if (value == NULL || value[0] == '\0')
			return false;
		errno = 0;
		end = NULL;
		parsed = std::strtol(value, &end, 10);
		if (errno != 0 || end == value || *end != '\0')
			return false;
		if (parsed < 1 || parsed > 65535)
			return false;
		port = static_cast<int>(parsed);
		return true;
	}

	void	printUsage(const char *programName)
	{
		if (programName == NULL || programName[0] == '\0')
			programName = "./ircbot";
		std::cerr << "Usage: " << programName
			<< " <host> <port> <password> <nickname> [channel]"
			<< std::endl;
	}
}

int	main(int argc, char **argv)
{
	int			port;
	std::string	channel;

	if (argc != 5 && argc != 6)
	{
		printUsage(argv[0]);
		return 1;
	}
	if (!parsePort(argv[2], port))
	{
		std::cerr << "Error: port must be an integer between 1 and 65535"
			<< std::endl;
		return 1;
	}
	if (argv[3][0] == '\0' || argv[4][0] == '\0')
	{
		std::cerr << "Error: password and nickname must not be empty"
			<< std::endl;
		return 1;
	}
	if (argc == 6)
		channel = argv[5];
	signal(SIGPIPE, SIG_IGN);
	IrcBot	bot(argv[1], port, argv[3], argv[4], channel);
	return bot.run();
}
