#include "xshmutex.hpp"

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>

namespace xshmutex
{

void xshmutex::_open()
{
    std::string fifo_name = "/tmp/.xshmutex_fifo_" + name_;
    data_.fd = open(fifo_name.c_str(), O_RDWR);
    if (data_.fd < 0)
    {
        throw std::runtime_error("could not init xshmutex: fifo open error " + std::to_string(errno));
    }
}

void xshmutex::_close()
{
    close(data_.fd);
}

void xshmutex::_init()
{
    std::string fifo_name = "/tmp/.xshmutex_fifo_" + name_;
    // Delete FIFO before proceeding
    unlink(fifo_name.c_str());
    int omask = umask(0);
    if (mkfifo(fifo_name.c_str(), 0666) == -1)
    {
        throw std::runtime_error("could not init xshmutex: fifo error " + std::to_string(errno));
    }
    _open();
    unlock();
    umask(omask);
}

void xshmutex::_destroy()
{
    std::string fifo_name = "/tmp/.xshmutex_fifo_" + name_;
    unlink(fifo_name.c_str());
}

void xshmutex::lock()
{
    char buf[1];
    assert(1 == read(data_.fd, buf, 1));
}

void xshmutex::unlock()
{
    assert(1 == write(data_.fd, "1", 1));
}

}
