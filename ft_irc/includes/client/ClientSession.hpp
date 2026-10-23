#ifndef FT_IRC_CLIENT_CLIENT_SESSION_HPP
# define FT_IRC_CLIENT_CLIENT_SESSION_HPP

# include <deque>
# include <string>

class ClientSession
{
public:
	explicit ClientSession(int fd);

	int					getFd() const;
	void				appendInput(const std::string &chunk);
	bool				popNextLine(std::string &line);
	void				enqueueReply(const std::string &message);
	bool				hasPendingOutput() const;
	std::string			dequeueOutput();

	void				setPasswordAccepted(bool accepted);
	void				setNickname(const std::string &nickname);
	const std::string	&getNickname() const;
	void				setUsername(const std::string &username);
	const std::string	&getUsername() const;
	void				setHostname(const std::string &hostname);
	const std::string	&getHostname() const;
	void				setRealname(const std::string &realname);
	bool				isRegistered() const;
	void				requestDisconnect();
	bool				shouldDisconnect() const;

private:
	void				updateRegistration();

	int						fd_;
	std::string				inputBuffer_;
	std::deque<std::string>	outputQueue_;
	bool					passwordAccepted_;
	bool					registered_;
	bool					disconnectRequested_;
	std::string				nickname_;
	std::string				username_;
	std::string				hostname_;
	std::string				realname_;
};

#endif
