#include "server/Server.hpp"
#include <cerrno>
#include <cstdlib>
#include <iostream>

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
			programName = "./ircserv";
		std::cerr << "Usage: " << programName << " <port> <password>" << std::endl;
	}
}

int	main(int argc, char **argv)
{
	int	port;

	if (argc != 3)
	{
		printUsage(argv[0]);
		return 1;
	}
	if (!parsePort(argv[1], port))
	{
		std::cerr << "Error: port must be an integer between 1 and 65535" << std::endl;
		return 1;
	}
	if (argv[2][0] == '\0')
	{
		std::cerr << "Error: password must not be empty" << std::endl;
		return 1;
	}
	Server	server(port, argv[2]);

	return server.run();
}
