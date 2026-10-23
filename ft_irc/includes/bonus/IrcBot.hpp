#ifndef FT_IRC_BONUS_IRC_BOT_HPP
# define FT_IRC_BONUS_IRC_BOT_HPP

# define BOT_RECV_BUFFER_SIZE 4096

# include <map>
# include <string>

class IrcBot
{
public:
	IrcBot(const std::string &host, int port, const std::string &password,
		const std::string &nickname, const std::string &channel);
	~IrcBot();

	int	run();

private:
	struct Message
	{
		std::string	prefix;
		std::string	command;
		std::string	target;
		std::string	text;
	};
	struct GameState
	{
		GameState();

		bool		active;
		std::string	answer;
		std::string	owner;
		int			guesses;
	};

	bool		connectToServer();
	bool		sendLine(const std::string &line);
	bool		readFromServer();
	void		handleLine(const std::string &line);
	void		handlePrivmsg(const Message &message);
	void		handlePing(const Message &message);
	bool		parseMessage(const std::string &line, Message &message) const;
	std::string	extractNick(const std::string &prefix) const;
	std::string	responseFor(const std::string &text,
					const std::string &sender,
					const std::string &context);
	std::string	handleHitAndBlow(const std::string &command,
					const std::string &rest,
					const std::string &sender,
					const std::string &context);
	std::string	startHitAndBlow(const std::string &sender,
					const std::string &context);
	std::string	guessHitAndBlow(const std::string &guess,
					const std::string &sender,
					const std::string &context);
	std::string	statusHitAndBlow(const std::string &context) const;
	std::string	giveUpHitAndBlow(const std::string &sender,
					const std::string &context);
	std::string	hitAndBlowRules() const;
	std::string	makeAnswer() const;
	bool		isValidGuess(const std::string &value) const;
	void		countHitAndBlow(const std::string &answer,
					const std::string &guess, int &hits, int &blows) const;
	void		closeSocket();

	std::string	host_;
	int			port_;
	std::string	password_;
	std::string	nickname_;
	std::string	channel_;
	int			socketFd_;
	std::string	inputBuffer_;
	bool		joined_;
	std::map<std::string, GameState>	games_;
};

#endif
