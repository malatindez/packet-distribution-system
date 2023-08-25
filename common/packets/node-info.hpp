#pragma once
#include "subsystems.hpp"
namespace _internal
{

    struct DiskInfo
    {
        std::string name;
        double available_bytes;
        double total_bytes;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &name;
            ar &available_bytes;
            ar &total_bytes;
        }
    };
}  // namespace _internal

MAL_PACKET_WEAVER_DECLARE_PACKET_WITHOUT_PAYLOAD(NodeInformationRequest, PacketSubsystemNodeInfo, 0,
                                                 120.0f)

MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_BODY_WITH_PAYLOAD(
    NodeInformationResponse, PacketSubsystemNodeInfo, 1, 120.0f, NodeInformationResponse();
    using DiskInfo = _internal::DiskInfo;
    (double, cpu_load), (double, gpu_load), (double, ram_load), (double, swap_load),
    (uint64_t, uptime), (int, process_count), (uint64_t, ram_bytes), (uint64_t, swap_bytes),
    (int, opened_files), (int, socket_count), (std::vector<DiskInfo>, disks_load))