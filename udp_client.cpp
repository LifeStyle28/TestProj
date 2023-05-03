#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <string_view>

namespace net = boost::asio;
namespace sys = boost::system;
using net::ip::udp;

using namespace std::literals;

const size_t max_buffer_size = 64;

std::array<char, max_buffer_size> to_request_v01(std::string& data)
{
    std::string str_command;
    std::stringstream ss(data);
    std::array<char, max_buffer_size> request{};
    request[0] = '0';
    request[1] = '1';
    int i = 1;
    while (ss >> str_command) {
        if (str_command.substr(0, 3) == "LED")
        {
            request[++i] = 'L';
            request[++i] = 'D';
            request[++i] = str_command[3];
            ++i;
            if (str_command.substr(4, 2) == "On")
                request[i] = 1 + '0';
            else
                request[i] = 0 + '0';
        }
    }
    return request;
}

void send_package(udp::socket& socket, udp::endpoint& endpoint, std::string& command)
{
    socket.send_to(net::buffer(to_request_v01(command)), endpoint);

    std::array<char, max_buffer_size> recv_buf;
    udp::endpoint sender_endpoint;
    size_t size = socket.receive_from(net::buffer(recv_buf), sender_endpoint);
    std::cout << "Server responded "sv << std::string_view(recv_buf.data(), size) << std::endl;
}

int main(int argc, const char *argv[]) {

    if (argc != 4)
    {
        std::cout << "Usage: "sv << "ip port (-i | command)"sv << std::endl;
        return 0;
    }

    try {
        net::io_context io_context;
        udp::socket socket(io_context, udp::v4());
        const uint16_t port = std::atoi(argv[2]);
        boost::system::error_code ec;
        auto endpoint = udp::endpoint(net::ip::address_v4::from_string(argv[1], ec), port);
        if (strcmp(argv[3], "-i") == 0)
        {
            std::string exit = "exit";
            while (true) {
                std::string command;
                std::cout << "> ";
                getline(std::cin, command);
                if (command == exit)
                {
                    break;
                }
                send_package(socket, endpoint, command);
            }
        }
        else
        {
            std::string command(argv[3]);
            send_package(socket, endpoint, command);
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}