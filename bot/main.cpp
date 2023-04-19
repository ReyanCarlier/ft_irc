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

int	main(int ac, char **av) {
	if (ac != 3) {
		std::cout << RED << "Usage: ./serverserv <port> <password>" << ENDL;
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
	int	status = connect(client_fd, reinterpret_cast<sockaddr*>(&serv_addr),
	sizeof(serv_addr));
	if (status < 0) {
		std::cerr << ERROR << "connect()" << ENDL;
		return (1);
	}
	char		buf_in[BUFFER_SIZE] = {0};
	std::string	buf_out = "CAP LS\nNICK bot\nUSER bot bot 127.0.0.1 :bot";
	std::string	str_in;

	/* First login */
	send(client_fd, buf_out.data(), buf_out.size(), 0);
	std::cout << BLUE << "Message sent." << ENDL;
	if (read(client_fd, buf_in, BUFFER_SIZE) == -1) {
		std::cerr << ERROR << "read()" << ENDL;
		return (1);
	}
	std::cout << GREEN << "buf_in=" << buf_in << ENDL;

	/* Bot */
	while (true)
	{
		bzero(buf_in, BUFFER_SIZE);
		if (read(client_fd, buf_in, BUFFER_SIZE) == -1) {
			std::cerr << ERROR << "read()" << ENDL;
			return (1);
		}
		std::cout << GREEN << "buf_in=" << buf_in << ENDL;
		str_in = buf_in;
		if (str_in.find("!die") != std::string::npos)
			break ;
	}
	close(client_fd);
	return (0);
}
