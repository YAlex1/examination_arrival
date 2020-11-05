#include <iostream>
#include <iomanip>
#include <sstream>
#include "data_view.h"
#include "ipclib.h"

using namespace Ipc;

/* */
int main ()
{
    /* Currently it is not used. May be used in further development */
    //DataView::DataViewClient<unsigned int> v;

    std::cout << "Waiting for a connection with the Data Generator process ..." << std::endl;
    /* Open IPC Communicator to read data from it. This object creation blocks untill the
     * data generator process will be launched.
     */
    auto ipccomm = Ipc::getIpcCommunicator<unsigned int>('r');
    std::cout << "Connection has been established." << std::endl;
 
    std::cout << std::endl;
    std::cout << "Current speed value is : " << std::endl;
    while(true)
    {
        /* print a received data to the console */
        std::cout << '\r'
                  << std::setw(3) << std::setfill(' ') << *ipccomm
                  << std::flush;
    }

    return 0;
}
