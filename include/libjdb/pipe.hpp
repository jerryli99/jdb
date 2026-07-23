#ifndef JDB_PIPE_HPP
#define JDB_PIPE_HPP

#include <vector>
#include <cstddef>

namespace jdb {
    class pipe {
    public: 
        //tells the pipe if it should automatically close the fd if process calls exec
        explicit pipe(bool close_on_exec);
        ~pipe();

        /*
        You might be wondering why implement getters here in header not in cpp file,
        ok, any function implemented directly inside the class definition block is
        automatically treated as inline by the compiler. So no need for CPU stack keeps
        jumping to different function addresses back and forth.
        */
        //put const here to not change the object this
        int get_read() const { return pipe_fds[read_fd]; }
        int get_write() const { return pipe_fds[write_fd]; }
        int release_read();
        int release_write();
        void close_read();
        void close_write();

        std::vector<std::byte> read();
        void write(std::byte* from, std::size_t bytes);


    private:
        int pipe_fds[2];
        //only visible to the class and no need to have copies of the values
        static constexpr unsigned read_fd = 0;
        static constexpr unsigned write_fd = 1;
    };
    
}


#endif