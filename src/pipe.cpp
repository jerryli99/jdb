#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <libjdb/pipe.hpp>
#include <libjdb/error.hpp>


jdb::pipe::pipe(bool close_on_exec) {
    if (pipe2(pipe_fds, (close_on_exec ? O_CLOEXEC : 0)) < 0) {
        error::send_errno("Pipe creation failed :(");
    }
}

jdb::pipe::~pipe() {
    close_read();
    close_write();
}

int jdb::pipe::release_read() {
    return std::exchange(pipe_fds[read_fd], -1);
}

int jdb::pipe::release_write() {
    return std::exchange(pipe_fds[write_fd], -1);
}

void jdb::pipe::close_read() {
    if (pipe_fds[read_fd] != -1) {
        close(pipe_fds[read_fd]);
        pipe_fds[read_fd] = -1;
    }
}

void jdb::pipe::close_write() {
    if (pipe_fds[write_fd] != -1) {
        close(pipe_fds[write_fd]);
        pipe_fds[write_fd] = -1;
    }
}

std::vector<std::byte> jdb::pipe::read() {
    char buffer[1024];
    int chars_read;
    if ((chars_read = ::read(pipe_fds[read_fd], buffer, sizeof(buffer))) < 0) {
        error::send_errno("Could not read from pipe");
    }

    auto bytes = reinterpret_cast<std::byte*>(buffer);
    return std::vector<std::byte>(bytes, bytes + chars_read);
}

void jdb::pipe::write(std::byte* from, std::size_t bytes) {
    if (::write(pipe_fds[write_fd], from, bytes) < 0) {
        error::send_errno("Could not write to pipe");
    }
}


