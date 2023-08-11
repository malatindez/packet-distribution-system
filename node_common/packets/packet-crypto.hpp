#pragma once
#include "../common/packet.hpp"
#include "../crypto/common.hpp"
#include "../crypto/sha.hpp"
#include <boost/endian/conversion.hpp>

namespace node_system::packet::crypto
{
    constexpr UniquePacketID DHKeyExchangeRequestPacketID = CreatePacketID(PacketSubsystemCrypto, 0x0000);
    constexpr UniquePacketID DHKeyExchangeResponsePacketID = CreatePacketID(PacketSubsystemCrypto, 0x0001);

    class DHKeyExchangeRequestPacket : public DerivedPacket<class DHKeyExchangeRequestPacket> {
    public:
        static constexpr UniquePacketID static_type = DHKeyExchangeRequestPacketID;
        static constexpr float time_to_live = 120.0f;
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
        static constexpr UniquePacketID static_type = DHKeyExchangeResponsePacketID;
        static constexpr float time_to_live = 120.0f;
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }
        [[nodiscard]] node_system::crypto::Hash get_hash() const
        {
            ByteArray arr;
            arr.append(public_key, salt, ByteArray::from_integral(boost::endian::little_to_native(static_type)));
            return node_system::crypto::SHA::ComputeHash(arr, node_system::crypto::Hash::HashType::SHA256);
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
    
    inline void RegisterDeserializers()
    {
        node_system::packet::PacketFactory::RegisterDeserializer<DHKeyExchangeRequestPacket>();
        node_system::packet::PacketFactory::RegisterDeserializer<DHKeyExchangeResponsePacket>();
    }

}