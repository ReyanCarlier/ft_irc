#include <header.hpp>

bool	is_port(char *av) {
	if (strlen(av) > 5)
		return (false);
	for (size_t i = 0; i < strlen(av); i++) {
		if (isdigit(av[i]) == false)
			return (false);
	}
	if (atoi(av) > MAX_TCP_PORT)
		return (false);
	return (true);
}

void	read_server(char *buf_in, int client_fd) {
	bzero(buf_in, BUFFER_SIZE);
	if (read(client_fd, buf_in, BUFFER_SIZE) == -1) {
		std::cerr << ERROR << "read()" << ENDL;
		exit(1);
	}
	std::cout << GREEN << "(buf_in)\n" << buf_in << ENDL;
}

/* Command ****************************************************************** */

void	bot_help(std::string const &str_in, int client_fd) {
	std::string				buf_out;
	std::stringstream		ss;
	std::string::size_type	pos = str_in.find(" PRIVMSG ");
	std::string::size_type	i;

	if (pos == std::string::npos)
		return ;
	ss << "PRIVMSG ";
	if (str_in[pos + 9] == '#') {
		pos += 9;
		i = pos;
		while (!isspace(str_in[i]))
			i++;
		ss << str_in.substr(pos, i - pos);
	} else {
		pos = 0;
		i = pos;
		while (!isspace(str_in[i]))
			i++;
		ss << str_in.substr(pos, i - pos);
	}
	buf_out = ss.str() + " :help : Show this text.\n";
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	buf_out = ss.str() + " :join <channel_name> : Join a channel.\n";
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	buf_out = ss.str() + " :ping : Say pong (not a real ping).\n";
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	buf_out = ss.str() + " :rainbow : Show a beautiful rainbow.\n";
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	buf_out = ss.str() + " :die <bot_password>: Close the bot.\n";
	send(client_fd, buf_out.data(), buf_out.size(), 0);
}

void	bot_join(std::string const &str_in, int client_fd) {
	std::string				buf_out;
	std::stringstream		ss;
	std::string::size_type	pos = str_in.find(" PRIVMSG ");
	std::string::size_type	i;

	if (pos == std::string::npos)
		return ;
	pos = str_in.find(" :!join");
	if (pos == std::string::npos)
		return ;
	pos += 7;
	while (str_in[pos] == ' ')
		pos++;
	if (str_in[pos] == '\r') {
		pos = str_in.find(" PRIVMSG ");
		ss << "PRIVMSG ";
		if (str_in[pos + 9] == '#') {
			pos += 9;
			i = pos;
			while (!isspace(str_in[i]))
				i++;
			ss << str_in.substr(pos, i - pos);
		} else {
			pos = 0;
			i = pos;
			while (!isspace(str_in[i]))
				i++;
			ss << str_in.substr(pos, i - pos);
		}
		ss << " :join : need an argument\n";
		buf_out = ss.str();
		send(client_fd, buf_out.data(), buf_out.size(), 0);
		return ;
	}
	i = pos;
	while (!isspace(str_in[i]))
		i++;
	ss << "JOIN #" << str_in.substr(pos, i - pos) << '\n';
	buf_out = ss.str();
	send(client_fd, buf_out.data(), buf_out.size(), 0);
}

void	bot_ping(std::string const &str_in, int client_fd) {
	std::string				buf_out;
	std::stringstream		ss;
	std::string::size_type	pos = str_in.find(" PRIVMSG ");
	std::string::size_type	i;

	if (pos == std::string::npos)
		return ;
	ss << "PRIVMSG ";
	if (str_in[pos + 9] == '#') {
		pos += 9;
		i = pos;
		while (!isspace(str_in[i]))
			i++;
		ss << str_in.substr(pos, i - pos);
	} else {
		pos = 0;
		i = pos;
		while (!isspace(str_in[i]))
			i++;
		ss << str_in.substr(pos, i - pos);
	}
	ss << " :pong\n";
	buf_out = ss.str();
	send(client_fd, buf_out.data(), buf_out.size(), 0);
}

void	bot_rainbow(std::string const &str_in, int client_fd) {
	std::string				buf_out;
	std::stringstream		ss;
	std::string::size_type	pos = str_in.find(" PRIVMSG ");
	std::string::size_type	i;

	if (pos == std::string::npos)
		return ;
	ss << "PRIVMSG ";
	if (str_in[pos + 9] == '#') {
		pos += 9;
		i = pos;
		while (!isspace(str_in[i]))
			i++;
		ss << str_in.substr(pos, i - pos);
	} else {
		pos = 0;
		i = pos;
		while (!isspace(str_in[i]))
			i++;
		ss << str_in.substr(pos, i - pos);
	}
	ss << " :" << RED << "█" << YELLOW << "█" << GREEN << "█" << CYAN << "█"
	<< BLUE << "█" << MAGENTA << "█" << RESET << '\n';
	buf_out = ss.str();
	for (size_t i = 0; i + buf_out.size() < BUFFER_SIZE; i += buf_out.size())
		send(client_fd, buf_out.data(), buf_out.size(), 0);
}

bool	bot_die(std::string const &str_in, int client_fd) {
	std::string				buf_out;
	std::stringstream		ss;
	std::string::size_type	pos = str_in.find(" PRIVMSG ");
	std::string::size_type	i;

	if (pos == std::string::npos)
		return (false);
	pos = str_in.find(" :!die");
	if (pos == std::string::npos)
		return (false);
	pos += 6;
	while (str_in[pos] == ' ')
		pos++;
	if (str_in[pos] == '\r') {
		pos = str_in.find(" PRIVMSG ");
		ss << "PRIVMSG ";
		if (str_in[pos + 9] == '#') {
			pos += 9;
			i = pos;
			while (!isspace(str_in[i]))
				i++;
			ss << str_in.substr(pos, i - pos);
		} else {
			pos = 0;
			i = pos;
			while (!isspace(str_in[i]))
				i++;
			ss << str_in.substr(pos, i - pos);
		}
		ss << " :die : need a password\n";
		buf_out = ss.str();
		send(client_fd, buf_out.data(), buf_out.size(), 0);
		return (false);
	}
	i = pos;
	while (!isspace(str_in[i]))
		i++;
	if (str_in.substr(pos, i - pos) != BOT_PASSWORD) {
		pos = str_in.find(" PRIVMSG ");
		ss << "PRIVMSG ";
		if (str_in[pos + 9] == '#') {
			pos += 9;
			i = pos;
			while (!isspace(str_in[i]))
				i++;
			ss << str_in.substr(pos, i - pos);
		} else {
			pos = 0;
			i = pos;
			while (!isspace(str_in[i]))
				i++;
			ss << str_in.substr(pos, i - pos);
		}
		ss << " :die : Bad password\n";
		buf_out = ss.str();
		send(client_fd, buf_out.data(), buf_out.size(), 0);
		return (false);
	}
	return (true);
}

int	main(int ac, char **av) {
	if (ac != 3) {
		std::cout << YELLOW << "Usage: ./bot <port> <password>" << ENDL;
		return (0);
	}
	if (is_port(av[1]) == false) {
		std::cerr << YELLOW << "Port \"" << av[1] << "\" is not good." << ENDL;
		return (0);
	}
	int	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		std::cerr << ERROR << "socket()" << ENDL;
		return (1);
	}
	int	opt = 1;
	if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
	sizeof(int)) == -1) {
		std::cerr << ERROR << "setsockopt()" << ENDL;
		return (1);
	}
	sockaddr_in	serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(av[1]));
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		std::cerr << ERROR << "inet_pton()" << ENDL;
		return (1);
	}
	if (connect(client_fd, reinterpret_cast<sockaddr*>(&serv_addr),
	sizeof(serv_addr)) < 0) {
		std::cerr << ERROR << "connect()" << ENDL;
		return (1);
	}

	/* Variable */
	char					buf_in[BUFFER_SIZE];
	std::string				str_in;
	std::string				buf_out;
	std::stringstream		ss;
	std::string::size_type	pos;
	
	/* Login */
	ss << "CAP LS\n";
	if (av[2][0])
		ss << "PASS " << av[2] << '\n';
	ss << "NICK " << BOT_NAME << "\nUSER " << BOT_NAME << ' ' << BOT_NAME
	<< " 127.0.0.1 :" << BOT_NAME << '\n';
	buf_out = ss.str();
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	read_server(buf_in, client_fd);

	/* Oper */
	ss.str("");
	ss << "OPER " << BOT_NAME << ' ' << OPER_PASSWORD << '\n';
	buf_out = ss.str();
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	read_server(buf_in, client_fd);
	read_server(buf_in, client_fd);

	/* Channel */
	ss.str("");
	ss << "JOIN #" << BOT_NAME << '\n';
	buf_out = ss.str();
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	read_server(buf_in, client_fd);
	read_server(buf_in, client_fd);

	/* Bot */
	std::cout << CYAN << "Entering the main loop." << ENDL;
	while (true)
	{
		read_server(buf_in, client_fd);
		str_in = buf_in;
		if (str_in.size() == 0)
			break ;
		str_in.erase(str_in.begin());
		pos = str_in.find(" :!");
		if (pos == std::string::npos)
			continue ;
		std::string	cmd = str_in.substr(pos + 3);

		/* Command */
		if (CMD_CMP(0, 4, "help") == 0 && isspace(cmd[4]))
			bot_help(str_in, client_fd);
		else if (CMD_CMP(0, 4, "join") == 0 && isspace(cmd[4]))
			bot_join(str_in, client_fd);
		else if (CMD_CMP(0, 4, "ping") == 0 && isspace(cmd[4]))
			bot_ping(str_in, client_fd);
		else if (CMD_CMP(0, 7, "rainbow") == 0 && isspace(cmd[7]))
			bot_rainbow(str_in, client_fd);
		else if (CMD_CMP(0, 3, "die") == 0 && isspace(cmd[3])) {
			if (bot_die(str_in, client_fd))
				break ;
		}
	}
	ss.str("");
	ss << "QUIT :leaving\n";
	buf_out = ss.str();
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	close(client_fd);
	return (0);
}
