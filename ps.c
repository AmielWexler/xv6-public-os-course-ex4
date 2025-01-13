#include "types.h"
#include "user.h"
#include "processInfo.h"

int
main(void)
{
    printf(1, "Total number of active processes: %d\n", getNumProc());
    printf(1, "Maximum PID: %d\n", getMaxPid());

    printf(1, "PID\tSTATE\tPPID\tSZ\tNFD\tNRSWITCH\n");

    for (int pid = 1; pid <= getMaxPid(); pid++) {
        struct processInfo pinfo;
        if (getProcInfo(pid, &pinfo) == 0) {
            const char *states[] = {"unused", "embryo", "sleeping", "runnable", "running", "zombie"};
            printf(1, "%d\t%s\t%d\t%d\t%d\t%d\n", pid, states[pinfo.state], pinfo.ppid, pinfo.sz, pinfo.nfd, pinfo.nrswitch);
        }
    }

    exit();
}
