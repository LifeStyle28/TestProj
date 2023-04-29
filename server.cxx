#include <array>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string_view>

template <std::size_t length>
std::ostream&
operator<<(std::ostream& os, const std::array<char, length>& container) {
  for (const char element : container) {
    if ('\0' == element) {
      break;
    }
    os << element;
  }
  return os;
}

int
main(int nargs, const char* args[nargs]) {
  using namespace boost;

  if (2 > nargs) {
    return 1;
  }

  int ntrans{std::atoi(args[1])};

  if (0 >= ntrans) {
    return 2;
  }

  asio::io_context context{};
  const auto server_addr{boost::asio::ip::make_address("192.168.0.11")};
  asio::ip::udp::endpoint endpoint{server_addr, 3333u};
  asio::ip::udp::socket socket{context, endpoint};

  std::array<char, 64u> buffer{};
  const std::string_view greeting{"Hello, boy!"};

  while (0 < ntrans--) {
    try {
      socket.receive_from(asio::buffer(buffer), endpoint);

    } catch (const system::system_error& error) {
      std::cerr << error.what() << std::endl;
    }

    std::cout << buffer << std::endl;
    socket.send_to(asio::buffer(greeting), endpoint);
  }
}
