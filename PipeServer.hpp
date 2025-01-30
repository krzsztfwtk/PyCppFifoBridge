#ifndef PIPE_SERVER_HPP
#define PIPE_SERVER_HPP

#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

class PipeServer {
public:
    PipeServer(const std::string& uuid) : running(false) {
        fifo_in = "/tmp/pycppfifobridge_in_" + uuid;
        fifo_out = "/tmp/pycppfifobridge_out_" + uuid;

        mkfifo(fifo_in.c_str(), 0666);
        mkfifo(fifo_out.c_str(), 0666);
    }

    void addFunction(const std::string &command, std::function<std::string()> func) {
        commands[command] = func;
    }

    void start() {
        running = true;
        listenerThread = std::thread(&PipeServer::listen, this);
    }

    void stop() {
        running = false;
        if (listenerThread.joinable())
            listenerThread.join();
    }

    ~PipeServer() {
        stop();
        unlink(fifo_in.c_str());
        unlink(fifo_out.c_str());
    }

private:
    std::unordered_map<std::string, std::function<std::string()>> commands;
    std::thread listenerThread;
    std::atomic<bool> running;
    std::string fifo_in, fifo_out;

void listen() {
    int fd_in = open(fifo_in.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd_in == -1) {
        std::cerr << "Failed to open input FIFO: " << strerror(errno) << std::endl;
        return;
    }

    int fd_out = -1;
    while (running) {
        fd_out = open(fifo_out.c_str(), O_WRONLY | O_NONBLOCK);
        if (fd_out == -1) {
            if (errno == ENXIO) {
                // No reader available, retry after a delay
                usleep(100000); // 100ms
                continue;
            } else {
                std::cerr << "Failed to open output FIFO: " << strerror(errno) << std::endl;
                close(fd_in);
                return;
            }
        } else {
            // Successfully opened output FIFO
            break;
        }
    }

    if (!running) {
        close(fd_in);
        if (fd_out != -1) close(fd_out);
        return;
    }

    char buffer[256];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(fd_in, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            std::string command(buffer);
            command.erase(command.find_last_not_of("\n") + 1);
            
            std::string response = commands.count(command) ? commands[command]() : "Unknown Command";
            response += "\n";
            write(fd_out, response.c_str(), response.size());
        } else if (bytes_read == -1 && errno == EAGAIN) {
            // No data available, sleep to prevent busy waiting
            usleep(100000); // 100ms
        } else if (bytes_read == 0 || bytes_read == -1) {
            // EOF or error, break the loop
            break;
        }
    }

    close(fd_in);
    close(fd_out);
}

};

#endif