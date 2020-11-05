#include "ipclib.h"

#pragma instantiate IpcComm<unsigned int>
#pragma instantiate IpcComm<unsigned int>::pointer_type getIpcCommunicator(char, std::stringig) 
#pragma instantiate std::ostream& operator<<(std::ostream& ostr, Ipc::IpcComm<unsigned int>& d)
#pragma instantiate std::ostream& operator>>(std::ostream& ostr, Ipc::IpcComm<unsigned int>& d)
#pragma instantiate std::stringstream& operator<<(std::stringstream& sstr, Ipc::IpcComm<unsigned int>& d)
#pragma instantiate std::stringstream& operator>>(std::stringstream& sstr, Ipc::IpcComm<unsigned int>& d)

//#define DEBUG
