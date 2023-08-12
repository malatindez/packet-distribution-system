#pragma once
#include "events/event.hpp"
#include "include/library-pch.hpp"

namespace node_system::events
{
    /**
     * @brief Event class for files changed.
     *
     * Represents an event indicating that one or more files have changed.
     */
    class FilesChangedEvent final : public Event
    {
    public:
        /**
         * @brief Constructs a FilesChangedEvent instance.
         *
         * @param files The list of changed file paths.
         */
        explicit FilesChangedEvent(std::vector<std::filesystem::path> &&files)
            : Event(EventType::FilesChanged, EventCategoryUtils), files_{ std::move(files) }
        {
        }

        FilesChangedEvent(FilesChangedEvent &&) = default;
        FilesChangedEvent(FilesChangedEvent const &) = default;
        FilesChangedEvent &operator=(FilesChangedEvent &&) = delete;
        FilesChangedEvent &operator=(FilesChangedEvent const &) = delete;

        /**
         * @brief Get the list of changed file paths.
         *
         * @return A reference to the list of changed file paths.
         */
        [[nodiscard]] inline std::vector<std::filesystem::path> const &files_changed()
        {
            return files_;
        }
        EVENT_CLASS_TYPE(FilesChanged)
    private:
        std::vector<std::filesystem::path> files_;
    };
} // namespace node_system::events
namespace utils
{
    /**
     * @brief A class for monitoring file changes and notifying listeners.
     */
    class FileWatcher
    {
    public:
        /**
         * @brief Constructs a FileWatcher instance with an event callback.
         *
         * @param event_callback The callback function to be invoked on file changes.
         */
        explicit FileWatcher(::node_system::EventCallbackFn const &event_callback)
            : event_callback_{ event_callback }
        {
        }

        /**
         * @brief Add a file path to watch for changes.
         *
         * @param path The file path to watch.
         * @param recursive Whether to watch subdirectories recursively.
         */
        void AddPathToWatch(std::filesystem::path const &path, bool recursive = true);

        /**
         * @brief Check for file updates and trigger event callback.
         */
        void OnUpdate();

    private:
        std::map<std::filesystem::path, std::filesystem::file_time_type> file_map_;
        ::node_system::EventCallbackFn event_callback_;
    };
} // namespace utils