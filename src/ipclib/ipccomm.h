#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <thread>


namespace Ipc
{

/* @brief A well known path to create FIFO IPC */
const std::string cIpcCommFifoPath = "/tmp/ipccomm";

/* 
 * @brief This class is intended to be an abstaraction for IPC communication.
 *        Under the hood it uses FIFO IPC - one of the most oldest and proven
 *        mechanisms in UNIX like systems.
 */
template<typename T>
class IpcComm
{
public:
    using pointer_type = std::unique_ptr<IpcComm<T> >;

    IpcComm(char mode, std::string fifo_path = cIpcCommFifoPath);
    ~IpcComm();
    
    IpcComm(const IpcComm&) = delete;
    IpcComm(IpcComm&&) = delete;
    IpcComm& operator=(const IpcComm& rhs) = delete;
    IpcComm& operator=(IpcComm&& rhs) = delete;

    template<typename U> 
    friend std::ostream& operator<<(std::ostream& ostr, IpcComm<U>& d);
    template<typename U>
    friend std::istream& operator>>(std::istream& istr, IpcComm<U>& d);
    template<typename U>
    friend std::stringstream& operator<<(std::stringstream& sstr, IpcComm<U>& d);
    template<typename U>
    friend std::stringstream& operator>>(std::stringstream& sstr, IpcComm<U>& d);

private:
    void openFifo();
    void closeFifo();
    T readFifo();
    void writeFifo(T val);
    int mFifoReadFd;
    int mFifoWriteFd;
    int mFifoDummyFd;
    std::string mFifoPath;
    char mMode;
}; // class IpcComm

/*
 *@brief Object's constraction method.
 *
 *@param mode       - 'r' for reading
 *                    'w' for writing
 *@param fifo_path  - file system path, well known and accessible
 *
 */
template<typename T>
IpcComm<T>::IpcComm(char mode, std::string fifo_path)
    : mFifoReadFd(-1)
    , mFifoWriteFd(-1)
    , mFifoDummyFd(-1)
    , mFifoPath(fifo_path)
    , mMode(mode)
{
#ifdef DEBUG
    std::cout << "IpcComm[ " << std::this_thread::get_id() << "]"
              << " constructor " 
              << mode << " " 
              << fifo_path
              << std::endl;
#endif
    try {
        openFifo();
    } catch (...){
        std::cerr << "IpcComm[ " << std::this_thread::get_id() << "]"
                  << "Couldn't open FIFO " << mFifoPath << std::endl;
    }
}

template<typename T>
IpcComm<T>::~IpcComm()
{
#ifdef DEBUG
    std::cout << "IpcComm[ " << std::this_thread::get_id() << "]" 
              << " destructor " 
              << std::endl;
#endif
    closeFifo();
}

/*
 * @brief This method creates and opens FIFO IPC end in one of the following modes:
 *        for reading or writing.
 *
 */
template<typename T>
void IpcComm<T>::openFifo()
{
    /* So we get the permissions we want */
    umask(0);
    
    /* Create FIFO */
    if (mkfifo(mFifoPath.c_str(), S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        std::cerr << "IpcComm::openFifo()[ " << std::this_thread::get_id() << "]"
                  << "ERROR on creating Fifo !" 
                  << mFifoPath 
                  << std::endl;
        throw;
    }
   
    if ('r' == mMode){
        /* open fifo for reading mode */
        mFifoReadFd = open(mFifoPath.c_str(), O_RDONLY);
        if (-1 == mFifoReadFd){
            std::cerr << "IpcComm::openFifo()[ " << std::this_thread::get_id() << "]"
                      << " ERROR on opening Fifo for reading mode!"
                      << std::endl;
            throw;
        }
    } else if ('w' == mMode){

        /* open dummy read descriptor */
        mFifoDummyFd = open(mFifoPath.c_str(), O_RDONLY | O_NONBLOCK);
        if (-1 == mFifoDummyFd){
            std::cerr << "IpcComm::openFifo()[ " << std::this_thread::get_id() << "]"
                      << " ERROR on opening Fifo for reading mode!"
                      << std::endl;
            throw;
        }

        /* open fifo for writing mode */
        mFifoWriteFd = open(mFifoPath.c_str(), O_WRONLY);
        if (-1 == mFifoWriteFd){
            std::cerr << "IpcComm::openFifo()[ " << std::this_thread::get_id() << "]"
                      << " ERROR on opening Fifo for writing mode!"
                      << std::endl;
            throw;
        }
        
    } else {
        std::cerr << "IpcComm[ " << std::this_thread::get_id() << "]"
                  << " ERROR unknown parameter mode = "
                  << mMode
                  << " of Fifo's opening mode is passed!"
                  << std::endl;
        throw;
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
        std::cerr << "IpcComm::openFifo()[ " << std::this_thread::get_id() << "]"
                  << " ERROR on signal(SIGPIPE, SIG_IGN) call !"
                  << std::endl;
        throw;
    }
}

/*
 *@brief Close file descriptors on object destruction.
 */
template<typename T>
void IpcComm<T>::closeFifo()
{
    /* close both ends of the fifo*/
    if (-1 != mFifoWriteFd) {
        close(mFifoWriteFd);
    }
    
    if (-1 != mFifoReadFd) {
        close(mFifoReadFd);
    }

    if (-1 != mFifoDummyFd) {
        close(mFifoDummyFd);
    }

}

/*
 *@brief Read data from FIFO IPC or detect an error.
 *
 */
template<typename T>
T IpcComm<T>::readFifo()
{
    T buf(0);

    if (-1 != mFifoReadFd) {
        int ret = read(mFifoReadFd, &buf, sizeof(T));
        if (ret && ret != sizeof(T)) {
            std::cerr << "IpcComm::readFifo()[ " << std::this_thread::get_id() << "]"
                      << " ERROR while reading from the fifo !"
                      << ret << " "
                      << buf 
                      << std::endl;
            throw;
        }
    }

    return buf;
}


/*
 *@brief Write data to FIFO IPC or detect an error.
 *
 */
template<typename T>
void IpcComm<T>::writeFifo(T val)
{
    T buf = val;

    if (-1 != mFifoWriteFd) {
        int ret = write(mFifoWriteFd, &buf, sizeof(T));
        if (ret && ret != sizeof(T)) {
            std::cerr << "IpcComm::readFifo()[ " << std::this_thread::get_id() << "]"
                      << " ERROR while writing to the fifo !"
                      << std::endl;
            throw;
        }
    }
}

/*
 *@brief Overwrite operator for outputing data to std::cout object
 */
template<typename U> 
std::ostream& operator<<(std::ostream& ostr, Ipc::IpcComm<U>& d)
{
    U buf;
    try {
        buf = d.readFifo();
        ostr << buf;
    } catch (...){
    }
    return ostr;
}

/*
 *@brief Overwrite operator to take an data input from std::cin object
 */
template<typename U>
std::istream& operator>>(std::istream& istr, Ipc::IpcComm<U>& d)
{
    try {
      U buf;
      istr >> buf;
      d.writeFifo(buf);
    } catch (...){
    }

    return istr;
}

/*
 *@brief Overwrite operator for outputting data to std::stringstream object
 */
template<typename U>
std::stringstream& operator<<(std::stringstream& sstr, Ipc::IpcComm<U>& d)
{
    U buf;
    try {
        buf = d.readFifo();
        sstr << buf;
    } catch (...){
    }
    return sstr;
}

/*
 *@brief Overwrite operator to take an data input from std::stringstream object 
 */
template<typename U>
std::stringstream& operator>>(std::stringstream& sstr, Ipc::IpcComm<U>& d)
{
    try {
      U buf;
      sstr >> buf;
      d.writeFifo(buf);
    } catch (...){
    }

    return sstr;
}

}; // namespace Ipc



