#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <string>
#include <string_view>
#include <sstream>
#include <boost/system/error_code.hpp>

namespace net = boost::asio;
using net::ip::udp;

constexpr int source_port = 3333;
constexpr int dest_port = 62510;
constexpr size_t message_size = 3;
constexpr size_t max_buffer_size = 1024;

enum Leds_states {
  OFF,
  ON,
  HIGH_Z
};

struct Leds_Control {
  uint8_t led1_state = HIGH_Z;
  uint8_t led2_state = HIGH_Z;
  uint8_t led3_state = HIGH_Z;
  //Бросает exception
  void add_token(const std::string& token) {
    //TODO
    //parsing
    if ((token.size() > 7) || (token.size() < 6) || (token.substr(0, 2) != "LD")) {
      std::string error = "Command \"" + token + "\" is wrong";
      std::cerr << error << std::endl;
      throw std::exception();
    }
    std::string str_state = token.substr(4);
    uint8_t state = HIGH_Z;
    if (str_state == "OFF")
      state = OFF;
    else if (str_state == "ON")
      state = ON;
    else {
      std::string error = "Command \"" + token + "\" is wrong";
      std::cerr << error << std::endl;
      throw std::exception();
    }
    char led_number = token[2];
    switch (led_number) {
      case '1':
        led1_state = state;
        break;
      case '2':
        led2_state = state;
        break;
      case '3':
        led3_state = state;
        break;
      default:       
        std::string error = "In command \"" + token + "\": " + led_number  + " is not a LED number";
        std::cerr << error << std::endl;
        throw std::exception();
    }
  }
  std::string to_string() {
    std::string message;
    message.push_back('0' + led1_state);
    message.push_back('0' + led2_state);
    message.push_back('0' + led3_state);
    return message;
  }
};

//Бросает exception (из-за .addtoken)
std::string parse_command(const std::string& command) {
  Leds_Control leds_control;
  std::stringstream command_stream(command);
  std::string token;
  while (command_stream >> token) {
    leds_control.add_token(token);
  }
  return leds_control.to_string();
}

constexpr std::string_view hint = "Usage: IP LDx/STATE LDy/STATE LDz/STATE\n"
                    "Where IP - Server adress\n"
                    "x, y, z - LED Number from 1 to 3\n"
                    "STATE = ON or OFF\n"
                    "Еhe number of LEDs can be less than 3\n\n"
                    "Example: 192.168.0.13 LD1/ON LD2/OFF\n\r";

int main(int argc, char* argv[])
{
  //Преобразуем аргументы программы в строку для того, чтобы работать как с getline
  std::stringstream program_input;
  for (int i = 1; i < argc; ++i) {
    program_input << argv[i] << ' '; //Не помешает ли пробел в конце потока (подумать)
  }
  
  //Добиваемся от пользователя правильного IP
  bool ip_valid = false;
  net::ip::address_v4 dest_ip;
  while (!ip_valid) {
    std::string ip4_str;
    if (program_input >> ip4_str) {
      try {
        dest_ip = net::ip::address_v4::from_string(ip4_str);
        ip_valid = true;
        break;
      } catch (...) {
        std::cerr << "Invalid IP! Type command with another IP:" << std::endl;
      }
    } else {
      std::cout << hint << std::endl;
    }

    std::string new_input;
    std::getline(std::cin, new_input);
    program_input.clear();
    program_input.str(new_input);
  }

  //Выкидываем IP из команды
  std::string command = program_input.str();
  std::string::size_type n = 0;
  n = command.find_first_not_of( " \t", n );
  n = command.find_first_of( " \t", n );
  command.erase( 0,  command.find_first_not_of( " \t", n ) );

  //Создаём сокет
  boost::asio::io_context io_context;
  udp::endpoint source_ep(udp::v4(), source_port);
  udp::socket socket(io_context, source_ep);

  std::array<char, message_size> send_buf;
  udp::endpoint remote_endpoint(dest_ip, dest_port);

  //Читаем и отправляем
  while (true) { 
    try {
      std::string message = parse_command(command);
      size_t size_to_copy = message.size();
      if (size_to_copy < max_buffer_size){
        for (int8_t i = 0; i < size_to_copy; ++i) {
          send_buf[i] = message[i];
        }
      }
      //TODO
      socket.send_to(net::buffer(send_buf), remote_endpoint, 0 /*TODO (заглушка)*/);
    } catch (std::exception& ex) {
      std::cerr << ex.what() <<std::endl;
    }
    std::getline(std::cin, command);
  }
  return 0;
}

