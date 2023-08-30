/*
 * Copyright (C) 2019-2020, Xilinx Inc - All rights reserved
 *
 * Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
 *
 * Xilinx Resouce Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "xrm_version.h"
#include "xrm_command_registry.hpp"
#include "xrm_tcp_server.hpp"
#include "xrm_system.hpp"

#include <syslog.h>
#include <signal.h>
#include <fstream>

using boost::asio::ip::tcp;

xrm::system* sys = NULL;
xrm::commandRegistry* registry = NULL;
boost::asio::io_service* ioService = NULL;
xrm::server* serv = NULL;
const uint16_t xrmPort = 9763;
uint32_t isExit = 0;
volatile uint32_t resetEvent = 0;
/*
 * need to be protected by lock
 */
void static sigbusHandler(int signNum, siginfo_t *siginfo, void *ptr) {
    std::ignore = siginfo;
    std::ignore = ptr;
    if (signNum == SIGBUS) {
	resetEvent = 1;    
    }

}

void resetEventFunc()
{
    boost::posix_time::seconds workTime(1);
    int32_t status;
    //  handle the reset devices 
    while (!isExit) {
        if (__sync_val_compare_and_swap(&resetEvent, 1, 0)) {
            sys->enterLock();
            int32_t numDevice = sys->getDeviceNumber();
            for (int32_t devId = 0; devId < numDevice; devId++) {
                status = sys->isDeviceOffline(devId);
                if (status == 1) {
                    sys->resetDevice(devId);
                }  
            }
            sys->exitLock();
        }
        boost::this_thread::sleep(workTime);
    }
}

int main(int argc, char* argv[]) {
    std::ignore = argc;
    std::ignore = argv;
    struct sigaction act;

    boost::thread workerThread(resetEventFunc); 
    try {
        setlogmask(LOG_UPTO(LOG_DEBUG));
        openlog("xrmd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
        syslog(LOG_NOTICE, "Starting XRM Daemon:");
        syslog(LOG_NOTICE, "    XRM Version = %s", XRM_VERSION_STRING);
        syslog(LOG_NOTICE, "    Git Branch = %s", XRM_GIT_BRANCH);
        syslog(LOG_NOTICE, "    Git Commit = %s", XRM_GIT_COMMIT_HASH);

        // Create the system object
        sys = new xrm::system;
        sys->initLock();
        sys->initSystem();

        // Create the commands
        registry = new xrm::commandRegistry;
        registry->registerAll(*sys);

        // Accept connections and process commands
        ioService = new boost::asio::io_service;
        serv = new xrm::server(*ioService, xrmPort);
        serv->setSystem(sys);
        serv->setRegistry(registry);

        memset (&act, 0, sizeof(act));
        act.sa_sigaction = sigbusHandler;
        sigfillset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;
        if (sigaction(SIGBUS, &act, 0))
            syslog(LOG_NOTICE, "Failed to setup SIGBUS handler");

        ioService->run();
    } catch (std::exception& e) {
        syslog(LOG_NOTICE, "Exception: %s", e.what());
    }
    isExit = 1;
    workerThread.join();
    if (serv != NULL) delete (serv);
    if (ioService != NULL) delete (ioService);
    if (registry != NULL) delete (registry);
    if (sys != NULL) delete (sys);
    return 0;
}
