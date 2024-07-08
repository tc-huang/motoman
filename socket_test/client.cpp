#include "control.hpp"

int main()
{
	// control::Communication communication;
	// communication.init();
	// communication.connect(PORT);
	// char message[BUFFER_SIZE];
	// memset(message, 0, sizeof(message));
	// sprintf(message, "Hello from client");
	// communication.send(message);

	// char buffer[BUFFER_SIZE];
	// memset(buffer, 0, sizeof(buffer));
	// communication.receive(buffer, BUFFER_SIZE);
	// std::cout << "Message from server: " << buffer << std::endl;

	// communication.close();

	control::Command command;
	command.power_on(PORT);
	command.write_tcp(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);
	command.write_joint(7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0);
	command.read_tcp();
	command.read_joint();
	command.power_off();
	
	return 0;
}
