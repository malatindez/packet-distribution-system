#pragma once
#include "mal-packet-weaver/packet.hpp"

namespace mql
{
    using MQL_char = int8_t;
    using MQL_uchar = uint8_t;

    using MQL_short = int16_t;
    using MQL_ushort = uint16_t;

    using MQL_int = int32_t;
    using MQL_uint = uint32_t;

    using MQL_long = int64_t;
    using MQL_ulong = uint64_t;

    using MQL_DateTime = std::chrono::time_point<std::chrono::milliseconds>;
}  // namespace mql
namespace boost::serialization
{
    template <class Archive>
    void serialize(Archive &ar, mql::MQL_DateTime &time_point, const unsigned int)
    {
        if (Archive::is_saving::value)
        {
            uint64_t uint_time =
                std::chrono::time_point_cast<std::chrono::milliseconds>(time_point)
                                    .time_since_epoch()
                                    .count();
            ar & uint_time;
        }
        else
        {
            uint64_t uint_time;
            ar &uint_time;
            time_point = ::mql::MQL_DateTime(std::chrono::milliseconds(uint_time));
        }
        
    }
}