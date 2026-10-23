#ifndef FT_IRC_BONUS_FILE_TRANSFER_HPP
# define FT_IRC_BONUS_FILE_TRANSFER_HPP

# include <string>

namespace Bonus
{
	class FileTransfer
	{
	public:
		static bool			isTransferMessage(const std::string &message);
		static bool			isDirectTarget(const std::string &target);
		static std::string	relayMessage(const std::string &senderNick,
								const std::string &target,
								const std::string &message);

	private:
		FileTransfer();
	};
}

#endif
