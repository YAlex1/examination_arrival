#include <iostream>
#include <sstream>
#include "data_gen.h"
#include "ipccomm.h"

/* Global variable which indicates that SIGINT system signal was caught */
static volatile std::atomic_bool gExitFlag(false);

/* Application exit signal handler */
static void sigHandler(int sig)
{
   gExitFlag = true;
}

/* */
int main ()
{
    /* Register system signal handler, reacting on <Ctrl-C> */
    if (signal(SIGINT, sigHandler) == SIG_ERR) {
        std::cerr << "Error, on registering signal's handler! " << std::endl; 
    }

    /* Create the Data Generator Engine object */
    DataGenerator::DataGenEngine<unsigned int> dataGen;

    /* Create the IPC Communicator oject in data writing mode enabled.
       The object's construction will be blocked untill data view client opens the same
       IPC Communicator for reading mode.
     */
    Ipc::IpcComm<unsigned int> ipccomm('w');

    /* An intermidiate object which allows to see the generated data value on the console */
    std::stringstream buf;

    std::cout << "Data genaration is started. Press <Ctrl+C> to quit." << std::endl;
    /* Running until SIGINT (Ctrl+C) system signal will caught */
    while(!gExitFlag)
    {
        using namespace DataGenerator;
        using namespace Ipc;
        
        /* Start a data value calculation in asynchronous way */
        dataGen.trigger();

        /* Check wether system signal for exit was caught */
        if (gExitFlag) break;
        
        /* Sleep this thread for a while */
        std::this_thread::sleep_for(std::chrono::milliseconds(DataGenerator::cWaitingTimePeriodValue));
        
        /* Check wether system signal for exit was caught */
        if (gExitFlag) break;
        
        /* Fetch the calculated data value */
        buf << dataGen;    

        /* Print it*/
        std::cout << buf.str() << "  " << std::flush ;

        /* And send it to the Data Viewer Client */
        buf >> ipccomm;
        
        /* Clear the intermediate buffer */
        buf.clear();
        buf.str("");
    }

    std::cout << std::endl;
    std::cout << "Data generation is done. Bye!"<< std::endl;
    return 0;
}
