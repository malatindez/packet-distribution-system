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
