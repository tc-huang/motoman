#ifndef CONTROL_HPP_
#define CONTROL_HPP_

#include <cmath>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define IP "192.168.255.10"
#define PORT 11000

#define BUFFER_SIZE 1024

#define WRITE_TCP 3
#define WRITE_JOINT 4
#define GET_CART_POSITION 2
#define GET_PULSE_POSITION 15

#define PULSE_PER_DEGREE_S  1492.765099
#define PULSE_PER_DEGREE_L -1517.038697
#define PULSE_PER_DEGREE_U  1264.198829
#define PULSE_PER_DEGREE_R -910.2222222
#define PULSE_PER_DEGREE_B  910.2230627
#define PULSE_PER_DEGREE_T -568.8894315

#define SPEED 100

namespace control
{
    class Communication
    {
        public:
            void init();
            void connect(short unsigned int port);
            void connect(const char* ip, short unsigned int port);
            void send(const char* message);
            int receive();
            void close();
            int receive_message[6] = {0};
        private:
            int socket_fd = -1;
    };

    class Command
    {
        public:
            Command();
            ~Command();
            void power_on(short unsigned int port);
            void power_on(const char* ip, short unsigned int port);
            void power_off();
            int write_tcp(float x, float y, float z, float rx, float ry, float rz, int speed);
            int write_joint(float b, float l, float r, float s, float t, float u, int speed);
            void read_tcp();
            void read_joint();
            void init();
            float radians[6] = {0.0};

        private:
            Communication communication;
            bool on = false;
            int pulse[6] = {0};
            float degrees[6] = {0.0};
            int cartesian_micro[6] = {0};
            float cartesian_milli[6] = {0.0};
            void pulse2degree();
            void degree2pulse();
            void degree2radian();
            void radian2degree();
            void milli2micro();
            void micro2milli();
    };
}
#endif  // CONTROL_HPP_