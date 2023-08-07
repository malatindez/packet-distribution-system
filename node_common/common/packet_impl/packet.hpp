#pragma once
#include "../common.hpp"
#include "../../utils/utils.hpp"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>

namespace node_system::packet
{
    using PacketSubsystemID = uint16_t;
    using PacketID = uint16_t;
    // (PacketSubsystem & 0xFFFF) << 16 | (Packet & 0xFFFF)
    using UniquePacketID = uint32_t;

    using PacketDeserializeFunc = std::function<std::unique_ptr<Packet>(const ByteView)>;

    // Predefined subsystems:
    constexpr PacketSubsystemID PacketSubsystemCrypto = 0x0001;
    constexpr PacketSubsystemID PacketSubsystemNode = 0x0002;
    constexpr PacketSubsystemID PacketSubsystemNetwork = 0x0003;
    constexpr PacketSubsystemID PacketSubsystemSystem = 0x0004;
    constexpr PacketSubsystemID PacketSubsystemUnknown = 0x0005;

    // TODO: use RBAC system to manage permissions
    enum class Permission : uint32_t
    {
        ANY = 0x0000UL,
        L1_NODE = 0x0001UL,
        L2_CORE_NODE = 0x0002UL,
        L2_PUBLIC_NODE = 0x0003UL,
        L2_ADMIN_NODE = 0x0004UL
    };
    
    constexpr uint32_t PacketSubsystemIDToUint32(PacketSubsystemID subsystem_type) noexcept
    {
        return utils::as_integer(subsystem_type);
    }

    constexpr PacketSubsystemID Uint32ToPacketSubsystemID(UniquePacketID subsystem_type) noexcept
    {
        return static_cast<PacketSubsystemID>((subsystem_type & 0xFFFF0000) >> 16);
    }

    constexpr PacketID Uint32ToPacketID(UniquePacketID subsystem_type) noexcept
    {
        return static_cast<PacketID>(subsystem_type & 0xFFFF);
    }

    constexpr UniquePacketID CreatePacketID(PacketSubsystemID subsystem_id, PacketID packet_id) noexcept
    {
        return (static_cast<UniquePacketID>(subsystem_id) << 16) | packet_id;
    }

    class Packet : public utils::non_copyable {
    private:
        static utils::Measurer<std::chrono::steady_clock> measurer;
    public:
        explicit Packet(const uint32_t type) : type(type), timestamp_{ measurer.elapsed() } {}
        Packet(Packet&&) = default;
        Packet& operator=(Packet&&) = default;
        virtual Permission get_permission() const = 0;

        virtual ~Packet() = default;
        virtual void serialize(ByteArray& buffer) const = 0;

        [[nodiscard]] float timestamp() const noexcept { return timestamp_; }

        const uint32_t type;
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version)
        {
            ar& timestamp_;
        }
        float timestamp_;
    };

    template<typename PacketType>
    class DerivedPacket : public Packet {
    public:
        DerivedPacket() : Packet(static_cast<uint32_t>(PacketType::static_type)) {}
        void serialize(ByteArray& buffer) const override {
            std::ostringstream oss;
            boost::archive::binary_oarchive oa(oss);
            oa << static_cast<const PacketType&>(*this);
            std::string const& s = oss.str();
            buffer.append(s);
        }

        static std::unique_ptr<Packet> deserialize(const ByteView buffer) {
            const auto char_view = buffer.as<char>();
            const std::string s(char_view, buffer.size());
            std::istringstream iss(s);
            boost::archive::binary_iarchive ia(iss);
            std::unique_ptr<PacketType> derived_packet(new PacketType);
            ia >> *derived_packet;
            return derived_packet;
        }

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<Packet>(*this);
        }
    };
} // namespace node_system