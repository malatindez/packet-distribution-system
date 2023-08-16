#pragma once
#include "mal-packet-weaver/crypto.hpp"
#include "mal-packet-weaver/packet.hpp"
#include <mal-packet-weaver/packet-impl/packet-macro.hpp>

constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemCrypto = 0x0001;

#ifndef MAL_PACKET_WEAVER_USE_MACROS
#define MAL_PACKET_WEAVER_USE_MACROS 1
#endif

#if MAL_PACKET_WEAVER_USE_MACROS
MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_PAYLOAD(DHKeyExchangeRequestPacket, PacketSubsystemCrypto, 0,
                                              120.0f, (mal_packet_weaver::ByteArray, public_key))

MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_BODY_WITH_PAYLOAD(DHKeyExchangeResponsePacket, PacketSubsystemCrypto, 1, 120.0f,
    [[nodiscard]] mal_packet_weaver::crypto::Hash get_hash() const { \
        mal_packet_weaver::ByteArray arr; \
        arr.append(public_key, salt, \
                   mal_packet_weaver::ByteArray::from_integral( \
                       boost::endian::little_to_native(static_unique_id))); \
        return mal_packet_weaver::crypto::SHA::ComputeHash( \
            arr, mal_packet_weaver::crypto::Hash::HashType::SHA256); \
    }
    , (mal_packet_weaver::ByteArray, public_key), (mal_packet_weaver::ByteArray, salt), (int, n_rounds),  (mal_packet_weaver::ByteArray, signature))

#else
/**
 * @brief Unique packet ID for DHKeyExchangeRequestPacket.
 */
constexpr mal_packet_weaver::UniquePacketID DHKeyExchangeRequestPacketID =
    mal_packet_weaver::CreatePacketID(PacketSubsystemCrypto, 0x0000);

/**
 * @brief Packet for Diffie-Hellman key exchange request.
 */
class DHKeyExchangeRequestPacket final
    : public mal_packet_weaver::DerivedPacket<class DHKeyExchangeRequestPacket>
{
public:
    static constexpr mal_packet_weaver::UniquePacketID static_unique_id = DHKeyExchangeRequestPacketID;
    static constexpr float time_to_live = 120.0f;
    mal_packet_weaver::ByteArray public_key;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
    {
        ar &public_key;
    }
    static mal_packet_weaver::PacketTypeRegistrationHelper<DHKeyExchangeRequestPacket> registration;
};
inline mal_packet_weaver::PacketTypeRegistrationHelper<DHKeyExchangeRequestPacket>
    DHKeyExchangeRequestPacket::registration;
/**
 * @brief Unique packet ID for DHKeyExchangeResponsePacket.
 */
constexpr mal_packet_weaver::UniquePacketID DHKeyExchangeResponsePacketID =
    mal_packet_weaver::CreatePacketID(PacketSubsystemCrypto, 0x0001);

/**
 * @brief Packet for Diffie-Hellman key exchange response.
 */
class DHKeyExchangeResponsePacket final
    : public mal_packet_weaver::DerivedPacket<class DHKeyExchangeResponsePacket>
{
public:
    static constexpr mal_packet_weaver::UniquePacketID static_unique_id = DHKeyExchangeResponsePacketID;
    static constexpr float time_to_live = 120.0f;

    /**
     * @brief Calculate the hash of the packet's contents.
     *
     * @return The calculated hash of the packet's contents.
     */
    [[nodiscard]] mal_packet_weaver::crypto::Hash get_hash() const
    {
        mal_packet_weaver::ByteArray arr;
        arr.append(public_key, salt,
                   mal_packet_weaver::ByteArray::from_integral(
                       boost::endian::little_to_native(static_unique_id)));
        return mal_packet_weaver::crypto::SHA::ComputeHash(
            arr, mal_packet_weaver::crypto::Hash::HashType::SHA256);
    }

    mal_packet_weaver::ByteArray public_key;
    mal_packet_weaver::ByteArray salt;
    int n_rounds;

    // Signature of the public key, salt and n_rounds.
    mal_packet_weaver::ByteArray signature;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
    {
        ar &public_key;
        ar &signature;
        ar &salt;
        ar &n_rounds;
    }
    static mal_packet_weaver::PacketTypeRegistrationHelper<DHKeyExchangeResponsePacket>
        registration;
};
inline mal_packet_weaver::PacketTypeRegistrationHelper<DHKeyExchangeResponsePacket>
    DHKeyExchangeResponsePacket::registration;
#endif
