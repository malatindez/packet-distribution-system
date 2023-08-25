#include "node-info.hpp"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>


#ifdef _WIN32
#include <Psapi.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <pdh.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "iphlpapi.lib")  // Link with the iphlpapi.lib library
#elif defined(__linux__)
#include <arpa/inet.h>
#include <linux/if_link.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


#elif defined(__APPLE__)
#include <arpa/inet.h>
#include <mach/mach.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/sysctl.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


#else
#error "Unsupported operating system"
#endif

double getCPULoad()
{
#ifdef _WIN32
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    lastCPU.QuadPart = kernelTime.dwLowDateTime |
                       (static_cast<unsigned long long>(kernelTime.dwHighDateTime) << 32);
    lastUserCPU.QuadPart =
        userTime.dwLowDateTime | (static_cast<unsigned long long>(userTime.dwHighDateTime) << 32);

    double percent = 0.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    ULARGE_INTEGER nowCPU, nowSysCPU, nowUserCPU;
    nowCPU.QuadPart = kernelTime.dwLowDateTime |
                      (static_cast<unsigned long long>(kernelTime.dwHighDateTime) << 32);
    nowUserCPU.QuadPart =
        userTime.dwLowDateTime | (static_cast<unsigned long long>(userTime.dwHighDateTime) << 32);

    auto sysDiff = nowCPU.QuadPart - lastCPU.QuadPart;
    auto userDiff = nowUserCPU.QuadPart - lastUserCPU.QuadPart;
    auto total = sysDiff + userDiff;

    percent = (total > 0) ? (userDiff * 100.0) / total : 0.0;
    return percent;
#elif defined(__linux__)
    std::ifstream statFile("/proc/stat");
    std::string line;
    std::getline(statFile, line);  // Read the first line

    long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    sscanf(line.c_str(), "cpu %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", &user, &nice,
           &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    long long idleTime = idle + iowait;
    long long totalTime = user + nice + system + idleTime + irq + softirq + steal;

    return (totalTime > 0) ? (100.0 * idleTime) / totalTime : 0.0;
#elif defined(__APPLE__)
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    host_cpu_load_info_data_t r_load;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO,
                        reinterpret_cast<host_info_t>(&r_load), &count) == KERN_SUCCESS)
    {
        unsigned long long totalTicks =
            r_load.cpu_ticks[CPU_STATE_USER] + r_load.cpu_ticks[CPU_STATE_SYSTEM] +
            r_load.cpu_ticks[CPU_STATE_NICE] + r_load.cpu_ticks[CPU_STATE_IDLE];
        unsigned long long idleTicks = r_load.cpu_ticks[CPU_STATE_IDLE];
        return (totalTicks > 0) ? (100.0 * idleTicks) / totalTicks : 0.0;
    }
    return 0.0;
#else
    return 0.0;
#endif
}

#ifdef _WIN32
double getGPULoad()
{
    // Implement GPU load retrieval for Windows using relevant libraries/APIs
    // You might need to use GPU-specific libraries like CUDA or DirectX to accurately measure GPU
    // load. This is a placeholder implementation.
    return 0.0;
}
#elif defined(__linux__)
double getGPULoad()
{
    // Implement GPU load retrieval for Linux using relevant libraries/APIs
    // You might need to use GPU-specific libraries like NVIDIA System Management Interface
    // (nvidia-smi) or AMD ROCm tools. This is a placeholder implementation.
    return 0.0;
}
#elif defined(__APPLE__)
double getGPULoad()
{
    // Implement GPU load retrieval for macOS using relevant libraries/APIs
    // You might need to use GPU-specific libraries provided by NVIDIA or AMD for macOS.
    // This is a placeholder implementation.
    return 0.0;
}
#endif

#ifdef _WIN32
double getRAMLoad()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG availPhysMem = memInfo.ullAvailPhys;
    return (totalPhysMem > 0) ? (100.0 * (totalPhysMem - availPhysMem)) / totalPhysMem : 0.0;
#elif defined(__linux__)
struct sysinfo memInfo;
sysinfo(&memInfo);
long long totalPhysMem = memInfo.totalram;
long long freePhysMem = memInfo.freeram + memInfo.bufferram + memInfo.sharedram;
return (totalPhysMem > 0) ? (100.0 * (totalPhysMem - freePhysMem)) / totalPhysMem : 0.0;
#elif defined(__APPLE__)
struct xsw_usage vmusage;
size_t size = sizeof(vmusage);
if (sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0) == 0)
{
    unsigned long long totalSwap = vmusage.xsu_total;
    unsigned long long usedSwap = vmusage.xsu_used;
    return (totalSwap > 0) ? (100.0 * usedSwap) / totalSwap : 0.0;
}
#endif
    return 0.0;
}

std::vector<NodeInformationResponse::DiskInfo> getDisksLoad()
{
    std::vector<NodeInformationResponse::DiskInfo> diskInfoList;
#ifdef _WIN32
    DWORD drives = GetLogicalDrives();
    char driveLetter = 'A';

    while (drives)
    {
        if (drives & 1)
        {
            std::string deviceName = std::string(1, driveLetter) + ":\\";
            ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
            if (GetDiskFreeSpaceExA(deviceName.c_str(), &freeBytesAvailable, &totalNumberOfBytes,
                                    &totalNumberOfFreeBytes))
            {
                double availableMemory = static_cast<double>(freeBytesAvailable.QuadPart);
                double totalMemory = static_cast<double>(totalNumberOfBytes.QuadPart);
                diskInfoList.emplace_back(
                    NodeInformationResponse::DiskInfo{ deviceName, availableMemory, totalMemory });
            }
        }
        drives >>= 1;
        driveLetter++;
    }
#elif defined(__linux__)
    // Linux implementation
    FILE* mountsFile = fopen("/proc/mounts", "r");
    if (mountsFile)
    {
        char line[512];
        while (fgets(line, sizeof(line), mountsFile))
        {
            char device[256], mountPoint[256];
            int matched = sscanf(line, "%255s %255s", device, mountPoint);
            if (matched == 2)
            {
                if (strstr(device, "/dev/"))
                {
                    double availableMemory =
                        getAvailableDiskMemory(mountPoint);               // Modify this function
                    double totalMemory = getTotalDiskMemory(mountPoint);  // Modify this function
                    diskInfoList.emplace_back(mountPoint, availableMemory, totalMemory);
                }
            }
        }
        fclose(mountsFile);
    }
#elif defined(__APPLE__)
    // macOS implementation
    FILE* mountsFile = fopen("/etc/fstab", "r");
    if (mountsFile)
    {
        char line[512];
        while (fgets(line, sizeof(line), mountsFile))
        {
            char device[256], mountPoint[256];
            int matched = sscanf(line, "%255s %255s", device, mountPoint);
            if (matched == 2)
            {
                double availableMemory =
                    getAvailableDiskMemory(mountPoint);               // Modify this function
                double totalMemory = getTotalDiskMemory(mountPoint);  // Modify this function
                diskInfoList.emplace_back(mountPoint, availableMemory, totalMemory);
            }
        }
        fclose(mountsFile);
    }
#endif
    return diskInfoList;
}

uint64_t getSystemUptimeMs()
{
#ifdef _WIN32
    return GetTickCount64();
#elif defined(__linux__)
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
#elif defined(__APPLE__)
    struct timeval boottime;
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };
    size_t size = sizeof(boottime);
    if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1)
    {
        time_t currentTime;
        time(&currentTime);
        return currentTime - boottime.tv_sec;
    }
#endif
    return 0;
}

int getProcessCount()
{
#ifdef _WIN32
    DWORD processIds[1024], bytesReturned;
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned))
    {
        return static_cast<int>(bytesReturned / sizeof(DWORD));
    }
#elif defined(__linux__) || defined(__APPLE__)
    int processCount = 0;
    std::ifstream procDir("/proc");
    while (procDir.good())
    {
        std::string filename;
        procDir >> filename;
        if (isdigit(filename[0]))
        {
            processCount++;
        }
    }
    return processCount;
#endif
    return 0;
}

uint64_t getTotalRAMBytes()
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPhys;

#elif defined(__linux__)
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    return memInfo.totalram;

#elif defined(__APPLE__)
    // There isn't a direct API to get the total physical memory on macOS. However, you can use
    // sysctl to get this information.
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    uint64_t totalMemory;
    size_t length = sizeof(totalMemory);
    sysctl(mib, 2, &totalMemory, &length, NULL, 0);
    return totalMemory;
#endif
}

double getSwapLoad()
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG availPageFile = memInfo.ullAvailPageFile;
    return (availPageFile > 0) ? (100.0 * availPageFile) / memInfo.ullTotalPageFile : 0.0;
#elif defined(__linux__)
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    long long freeSwap = memInfo.freeswap;
    return (freeSwap > 0) ? (100.0 * freeSwap) / memInfo.totalswap : 0.0;
#elif defined(__APPLE__)
    struct xsw_usage vmusage;
    size_t size = sizeof(vmusage);
    if (sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0) == 0)
    {
        unsigned long long freeSwap = vmusage.xsu_avail;
        unsigned long long totalSwap = vmusage.xsu_total;
        return (totalSwap > 0) ? (100.0 * freeSwap) / totalSwap : 0.0;
    }
    return 0.0;
#endif
}
uint64_t getTotalSwapBytes()
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPageFile;

#elif defined(__linux__)
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    return memInfo.totalswap;

#elif defined(__APPLE__)
    struct xsw_usage vmusage;
    size_t size = sizeof(vmusage);
    if (sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0) == 0)
    {
        return vmusage.xsu_total;
    }
    return 0;
#endif
}

int getOpenFileHandleCount()
{
#ifdef _WIN32
    // Windows Implementation
    DWORD handleCount;
    if (!GetProcessHandleCount(GetCurrentProcess(), &handleCount))
    {
        // Handle error
        return -1;
    }
    return static_cast<int>(handleCount);
#elif defined(__linux__)
    // Linux Implementation
    const std::string procFilePath = "/proc/self/fd";
    struct dirent* dirEntry;
    int handleCount = 0;

    DIR* dir = opendir(procFilePath.c_str());
    if (!dir)
    {
        // Handle error
        return -1;
    }

    while ((dirEntry = readdir(dir)) != nullptr)
    {
        // Skip . and .. entries
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            handleCount++;
        }
    }
    closedir(dir);
    return handleCount;
#elif defined(__APPLE__)
    // macOS Implementation
    int handleCount = proc_listallpids(nullptr, 0);
    if (handleCount < 0)
    {
        // Handle error
        return -1;
    }
    return handleCount;
#else
#error "Unsupported operating system"
#endif
}

#ifdef _WIN32
#include <winsock2.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/socket.h>
#endif

int getOpenSocketCount()
{
    int openSockets = 0;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
    {
        MIB_TCPTABLE2* pTcpTable;
        ULONG ulSize = 0;

        // Get the required size for the TCP table
        if (GetTcpTable2(nullptr, &ulSize, TRUE) == ERROR_INSUFFICIENT_BUFFER)
        {
            pTcpTable = (MIB_TCPTABLE2*)malloc(ulSize);
            if (pTcpTable)
            {
                if (GetTcpTable2(pTcpTable, &ulSize, TRUE) == NO_ERROR)
                {
                    for (DWORD i = 0; i < pTcpTable->dwNumEntries; ++i)
                    {
                        if (pTcpTable->table[i].dwState == MIB_TCP_STATE_ESTAB)
                        {
                            ++openSockets;
                        }
                    }
                }
                free(pTcpTable);
            }
        }

        WSACleanup();
    }

#elif defined(__linux__) || defined(__APPLE__)
    // Use the Linux/POSIX-specific approach to count open sockets
    DIR* procDir = opendir("/proc");
    if (procDir)
    {
        struct dirent* entry;
        while ((entry = readdir(procDir)))
        {
            if (isdigit(entry->d_name[0]))
            {
                int pid = atoi(entry->d_name);

                char procPath[256];
                snprintf(procPath, sizeof(procPath), "/proc/%d/fd", pid);
                struct dirent* fileEntry;

                DIR* fdDir = opendir(procPath);
                if (fdDir)
                {
                    while ((fileEntry = readdir(fdDir)))
                    {
                        if (isdigit(fileEntry->d_name[0]))
                        {
                            int fd = atoi(fileEntry->d_name);
                            struct stat sockStat;
                            char fdPath[256];

                            snprintf(fdPath, sizeof(fdPath), "/proc/%d/fd/%d", pid, fd);
                            if (lstat(fdPath, &sockStat) == 0 && S_ISSOCK(sockStat.st_mode))
                            {
                                openSockets++;
                            }
                        }
                    }
                    closedir(fdDir);
                }
            }
        }
        closedir(procDir);
    }
#endif

    return openSockets;
}

NodeInformationResponse::NodeInformationResponse()
{
    cpu_load = getCPULoad();
    gpu_load = getGPULoad();
    ram_load = getRAMLoad();
    swap_load = getSwapLoad();
    uptime = getSystemUptimeMs();
    process_count = getProcessCount();
    ram_bytes = getTotalRAMBytes();
    swap_bytes = getTotalSwapBytes();
    opened_files = getOpenFileHandleCount();
    socket_count = getOpenSocketCount();
    disks_load = getDisksLoad();
}
