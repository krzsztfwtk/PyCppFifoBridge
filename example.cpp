#include "PipeServer.hpp"

std::string hello() { return "Hello, World!"; }
std::string calculate() { return std::to_string(8 * 8); }
std::string bye() { return "Good Bye!"; }

int main() {
    // 12345 is an example UUID. If you want to run multiple Python clients
    // with their paired C++ servers inside your OS, each connection
    // must use a unique UUID to prevent conflicts between different instances.
    PipeServer server("12345");

    server.addFunction("hello", hello);
    server.addFunction("calculate", calculate);
    server.addFunction("bye", bye);

    std::cout << "C++ Pipe Server Started...\n"
              << "Type 'exit' to quit.\n";

    server.start();

    while (true) {
        std::string userInput;
        if (!std::getline(std::cin, userInput)) {
            break;
        }

        if (userInput == "exit") {
            server.stop();
            break;
        }
    }

    std::cout << "Exiting...\n";
    return 0;
}