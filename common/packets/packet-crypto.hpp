#pragma once
#include "mal-packet-weaver/crypto.hpp"
#include "subsystems.hpp"

MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_PAYLOAD(DHKeyExchangeRequestPacket, PacketSubsystemCrypto, 0,
                                              120.0f, (mal_packet_weaver::ByteArray, public_key))

MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_BODY_WITH_PAYLOAD(
    DHKeyExchangeResponsePacket, PacketSubsystemCrypto, 1, 120.0f,
    [[nodiscard]] mal_packet_weaver::crypto::Hash get_hash() const {
        mal_packet_weaver::ByteArray arr;
        arr.append(public_key, salt,
                   mal_packet_weaver::ByteArray::from_integral(
                       boost::endian::little_to_native(static_unique_id)));
        return mal_packet_weaver::crypto::SHA::ComputeHash(
            arr, mal_packet_weaver::crypto::Hash::HashType::SHA256);
    },
    (mal_packet_weaver::ByteArray, public_key), (mal_packet_weaver::ByteArray, salt),
    (int, n_rounds), (mal_packet_weaver::ByteArray, signature))
