#pragma once
#include "include/library-pch.hpp"

/**
 * @brief Macro to define the class type and name for an event.
 *
 * This macro defines a static constexpr event_name member variable and a name() method
 * for the given event type. The name() method returns the string representation of the event type.
 *
 * @param event_type The name of the event type.
 */
#define EVENT_CLASS_TYPE(event_type)                            \
    static constexpr std::string_view event_name = #event_type; \
    [[nodiscard]] std::string_view name() const override { return event_name; }

namespace node_system::events
{
    /**
     * @brief Enum representing different types of events.
     */
    enum class EventType : uint32_t
    {
        None = 0,
        FilesChanged = 1
    };

    /**
     * @brief Enum representing different categories of events.
     */
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryUtils = 1 << 1,
        EventCategoryNetwork = 1 << 2,
    };

    /**
     * @brief Base class for all events.
     */
    class Event
    {
    public:
        /**
         * @brief Constructor for the Event class.
         *
         * @param type The EventType of the event.
         * @param flags The category flags of the event.
         */
        Event(EventType type, uint8_t flags) : kType{ type }, kCategoryFlags{ flags } {}
        
        /**
         * @brief Virtual destructor for the Event class.
         */
        virtual ~Event() = default;

        /**
         * @brief Returns the EventType of the event.
         *
         * @return The EventType of the event.
         */
        [[nodiscard]] EventType type() const noexcept { return kType; }
        
        /**
         * @brief Returns the category flags of the event.
         *
         * @return The category flags of the event.
         */
        [[nodiscard]] uint8_t category_flags() const noexcept { return kCategoryFlags; };
        
        /**
         * @brief Checks if the event is in the specified category.
         *
         * @param category The category to check against.
         * @return True if the event is in the specified category, false otherwise.
         */
        [[nodiscard]] inline bool in_category(uint8_t category) const noexcept
        {
            return (category & category_flags());
        }
        /**
         * @brief Returns the name of the event.
         *
         * @return The name of the event.
         */
        [[nodiscard]] virtual std::string_view name() const = 0;
        
        /**
         * @brief Returns the string representation of the event.
         *
         * @return The string representation of the event.
         */
        [[nodiscard]] virtual std::string to_string() const noexcept
        {
            return std::string(name());
        }

        /**
         * @brief Indicates whether the event has been handled.
         */
        bool handled = false;

        /**
         * @brief The EventType of the event.
         */
        const EventType kType;

        /**
         * @brief The category flags of the event.
         */
        const uint8_t kCategoryFlags;
    };
} // namespace node_system::events

namespace node_system
{
    /**
     * @brief Alias for the event callback function.
     */
    using EventCallbackFn = std::function<void(events::Event&)>;
} // namespace node_system