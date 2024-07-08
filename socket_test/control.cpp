#include "control.hpp"

namespace control
{
    void Communication::init()
    {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            std::cerr << "Socket creation error" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void Communication::connect(short unsigned int port)
    {
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = INADDR_ANY; //inet_addr(IP);
        if (::connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Connection Failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void control::Communication::send(const char* message)
    {
        int status = ::send(socket_fd, message, strlen(message), 0);
        if (status < 0) {
            std::cerr << "Send failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    int Communication::receive()
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        int recv_bytes = ::recv(socket_fd, buffer, BUFFER_SIZE, 0);
        if (recv_bytes < 0) {
            std::cerr << "Receive failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        else std::cout << "Server response: " << buffer << std::endl;

        int i = 0;
        int j = 0;
        int k = 0;
        char abuff[1024];
        memset(abuff, 0, sizeof(abuff));
        
        for (i = 0; i < recv_bytes; i++) {
            if (buffer[i] == ',') {
                receive_message[k] = atoi(abuff); k++;
                j = 0; memset(abuff, 0, sizeof(abuff));
            }
            else if (buffer[i] == ';') {
                receive_message[5] = atoi(abuff);
                j = 0; memset(abuff, 0, sizeof(abuff));
                return 0;
            }
            else if (buffer[i] <= 0x39 && buffer[i] >= 0x30) {
                abuff[j] = buffer[i]; j++;
            }
            else if (buffer[i] == '-') {
                abuff[j] = buffer[i]; j++;
            }
            else if (buffer[i] == 'n') {
                return 2;
            }
        }
        return 0;
    }

    void Communication::close()
    {
        ::close(control::Communication::socket_fd);
    }

    Command::Command()
    {
        communication.init();
    }

    Command::~Command()
    {
        communication.close();
        on = false;
    }

    void Command::power_on(short unsigned int port)
    {
        communication.connect(port);
        on = true;
    }

    void Command::power_off()
    {
        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(BUFFER_SIZE));
        sprintf(message, "%c %d %d %d %d %d %d %d", 'c', 0, 0, 0, 0, 0, 0, 0);
        communication.send(message);
    }

    int Command::write_tcp(float x, float y, float z, float rx, float ry, float rz, int speed)
    {
        cartesian_milli[0] = x;
        cartesian_milli[1] = y;
        cartesian_milli[2] = z;
        cartesian_milli[3] = rx;
        cartesian_milli[4] = ry;

        milli2micro();

        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(BUFFER_SIZE));
        sprintf(message, "%d %d %d %d %d %d %d %d", cartesian_micro[0], cartesian_micro[1], cartesian_micro[2], cartesian_micro[3], cartesian_micro[4], cartesian_micro[5], speed, WRITE_TCP);
        communication.send(message);
        return communication.receive();
    }

    int Command::write_joint(float s, float l, float u, float r, float b, float t, int speed)
    {
        radians[0] = s;
        radians[1] = l;
        radians[2] = u;
        radians[3] = r;
        radians[4] = b;
        radians[5] = t;

        radian2degree();
        degree2pulse();

        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(BUFFER_SIZE));
        
        sprintf(message, "%d %d %d %d %d %d %d %d", pulse[0], pulse[1], pulse[2], pulse[3], pulse[4], pulse[5], speed, WRITE_JOINT);
        communication.send(message);
        return communication.receive();
    }

    void Command::read_tcp()
    {
        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(BUFFER_SIZE));
        sprintf(message, "%d %d %d %d %d %d %d %d", 0, 0, 0, 0, 0, 0, 0, GET_CART_POSITION);
        communication.send(message);
        communication.receive();
        std::cout << "TCP position: ";
        for (int i = 0; i < 6; i++) {
            cartesian_micro[i] = communication.receive_message[i];
            std::cout << cartesian_micro[i] << " ";
        }
        std::cout << std::endl;
        micro2milli();
    }

    void Command::read_joint()
    {
        char message[BUFFER_SIZE];
        memset(message, 0, sizeof(BUFFER_SIZE));
        sprintf(message, "%d %d %d %d %d %d %d %d", 0, 0, 0, 0, 0, 0, 0, GET_PULSE_POSITION);
        communication.send(message);
        communication.receive();
        std::cout << "Joint position: ";
        for (int i = 0; i < 6; i++) {
            pulse[i] = communication.receive_message[i];
            std::cout << pulse[i] << " ";
        }
        std::cout << std::endl;
        pulse2degree();
        degree2radian();
    }

    void Command::pulse2degree()
    {
        degrees[0] = pulse[0] / PULSE_PER_DEGREE_S;
        degrees[1] = pulse[1] / PULSE_PER_DEGREE_L;
        degrees[2] = pulse[2] / PULSE_PER_DEGREE_U;
        degrees[3] = pulse[3] / PULSE_PER_DEGREE_R;
        degrees[4] = pulse[4] / PULSE_PER_DEGREE_B;
        degrees[5] = pulse[5] / PULSE_PER_DEGREE_T;
    }

    void Command::degree2pulse()
    {
        pulse[0] = degrees[0] * PULSE_PER_DEGREE_S;
        pulse[1] = degrees[1] * PULSE_PER_DEGREE_L;
        pulse[2] = degrees[2] * PULSE_PER_DEGREE_U;
        pulse[3] = degrees[3] * PULSE_PER_DEGREE_R;
        pulse[4] = degrees[4] * PULSE_PER_DEGREE_B;
        pulse[5] = degrees[5] * PULSE_PER_DEGREE_T;
    }

    void Command::degree2radian()
    {
        radians[0] = degrees[0] * M_PI / 180;
        radians[1] = degrees[1] * M_PI / 180;
        radians[2] = degrees[2] * M_PI / 180;
        radians[3] = degrees[3] * M_PI / 180;
        radians[4] = degrees[4] * M_PI / 180;
        radians[5] = degrees[5] * M_PI / 180;
    }

    void Command::radian2degree()
    {
        degrees[0] = radians[0] * 180 / M_PI;
        degrees[1] = radians[1] * 180 / M_PI;
        degrees[2] = radians[2] * 180 / M_PI;
        degrees[3] = radians[3] * 180 / M_PI;
        degrees[4] = radians[4] * 180 / M_PI;
        degrees[5] = radians[5] * 180 / M_PI;
    }

    void Command::milli2micro()
    {
        for (int i = 0; i < 6; i++) {
            cartesian_micro[i] = cartesian_milli[i] * 1000;
        }
    }

    void Command::micro2milli()
    {
        for (int i = 0; i < 6; i++) {
            cartesian_milli[i] = cartesian_micro[i] / 1000;
        }
    }
}