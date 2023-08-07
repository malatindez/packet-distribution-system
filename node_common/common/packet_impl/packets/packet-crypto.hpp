#pragma once
#include "../packet.hpp"
#include "../core/crypto/common.hpp"
#include "../core/crypto/sha.hpp"
namespace node_system::packet::crypto
{
    constexpr PacketID DHKeyExchangeRequestPacketID = 0x0000;
    constexpr PacketID DHKeyExchangeResponsePacketID = 0x0001;

    class DHKeyExchangeRequestPacket : public DerivedPacket<class DHKeyExchangeRequestPacket> {
    public:
        static constexpr UniquePacketID static_type = CreatePacketID(PacketSubsystemCrypto, DHKeyExchangeRequestPacketID);
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }

        ByteArray public_key;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class DHKeyExchangeRequestPacket>>(*this);
            ar& public_key;
        }
    };

    class DHKeyExchangeResponsePacket : public DerivedPacket<class DHKeyExchangeResponsePacket> {
    public:
        static constexpr UniquePacketID static_type = CreatePacketID(PacketSubsystemCrypto, DHKeyExchangeResponsePacketID);
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }
        [[nodiscard]] crypto::Hash get_hash() const
        {
            ByteArray arr;
            arr.append(public_key, salt, ByteArray::from_integral(boost::endian::little_to_native(static_type)));
            return crypto::SHA::ComputeHash(arr, crypto::Hash::HashType::SHA256);
        }

        ByteArray public_key;
        ByteArray salt;
        int n_rounds;

        // Signature of the public key, salt and n_rounds.
        ByteArray signature;
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class DHKeyExchangeResponsePacket>>(*this);
            ar& public_key;
            ar& signature;
            ar& salt;
            ar& n_rounds;
        }
    };
}