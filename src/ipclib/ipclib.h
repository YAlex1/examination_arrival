#pragma once

#include <memory>
#include "ipccomm.h"


namespace Ipc
{
/*
 *@brief This is factory method, which creates an instance of IPC Communicator object
 *       in dynamic memory (heap) region. 
 */
template<typename T>
typename IpcComm<T>::pointer_type getIpcCommunicator(char mode, std::string fifo_path = cIpcCommFifoPath)
{
    return std::unique_ptr<IpcComm<T> >(new IpcComm<T>(mode, fifo_path));   
}

} // namespace Ipc
