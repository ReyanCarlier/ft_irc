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

void	bot_ping(std::string const &str_in, int client_fd) {
	std::stringstream		ss;
	std::string				buf_out;

	std::string::size_type	pos = str_in.find(" PRIVMSG ");
	if (pos == std::string::npos)
		return ;
	ss << "PRIVMSG ";
	int	i;
	if (str_in[pos + 9] == '#') {
		pos += 9;
		for (i = pos; str_in[i] != ' ';)
			i++;
		ss << str_in.substr(pos, i - pos);
	} else {
		// TODO
	}
	ss << " :pong\n";
	buf_out = ss.str();
	send(client_fd, buf_out.data(), buf_out.size(), 0);
}

int	main(int ac, char **av) {
	if (ac != 3) {
		std::cout << RED << "Usage: ./bot <port> <password>" << ENDL;
		return (0);
	}
	if (is_port(av[1]) == false) {
		std::cerr << RED << "Port \"" << av[1] << "\" is not good." << ENDL;
		return (0);
	}
	int	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
	std::cout << CYAN << "Entering the loop." << ENDL;
	while (true)
	{
		read_server(buf_in, client_fd);
		str_in = buf_in;
		str_in.erase(str_in.begin());
		pos = str_in.find(" :!");
		if (pos == std::string::npos)
			continue ;
		std::string	cmd = str_in.substr(pos + 3);
		if (CMD_CMP("join\r\n") == 0 || CMD_CMP(0, 5, "join ") == 0)
			continue ;
		else if (CMD_CMP("ping\r\n") == 0 || CMD_CMP(0, 5, "ping ") == 0)
			bot_ping(buf_in, client_fd);
		else if (CMD_CMP("die\r\n") == 0 || CMD_CMP(0, 5, "die ") == 0)
			break ;
	}
	close(client_fd);
	return (0);
}
