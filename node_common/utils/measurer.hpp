#pragma once
#include "debug.hpp"
#include "timer.hpp"
namespace utils
{
    /**
     * @brief A class for measuring and logging the execution time of functions.
     * @tparam clock The clock type to use for timing (e.g., std::chrono::steady_clock).
     */
    template <class clock> struct Measurer
    {
    public:
        struct Entry
        {
            float entry_time;
            float elapsed;
        };

        /**
         * @brief Construct a Measurer object.
         * @param s A string identifier for the measurer.
         * @param log_automatically Whether to log measurements automatically.
         * @param time_to_flush Time interval for automatic log flushing (in seconds).
         * @param location The source location (optional) for debug logging.
         */
#ifndef ENGINE_NO_SOURCE_LOCATION
        Measurer(std::string_view s = "Measurer", bool log_automatically = true,
                 float time_to_flush = 30,
                 std::source_location location = std::source_location::current())
            : log_automatically(log_automatically), time_to_flush(time_to_flush)
        {
            output = CurrentSourceLocation(location) + std::basic_string(s) + " ";
        }
#else
        Measurer(std::string_view s = "Measurer", bool log_automatically = true,
                 float time_to_flush = 30)
            : time_to_flush(time_to_flush), log_automatically(log_automatically)
        {
            output = s;
        }
#endif

        /**
         * @brief Start measuring execution time.
         */
        void begin() { measure.reset_to_now(); }

        /**
         * @brief End measurement and log the result.
         * @return The elapsed time in seconds.
         */
        float end()
        {
            const float t = measure.elapsed();
            entries.push_back({ entry_time.elapsed(), measure.elapsed() });
            if (log_automatically && flush.elapsed() > time_to_flush)
            {
                log();
                flush.reset();
            }
            return t;
        }

        /**
         * @brief Log the accumulated measurements.
         */
        void log()
        {
            const size_t entries_amount = entries.size() - index;
            index = entries.size() - 1;

            const float average = avg();
            const float avg_over_the_flush = avg(entries_amount);

            std::stringstream out;
            out << output << std::endl;
            out << "Amount of calls over the last " << std::setprecision(3) << flush.elapsed()
                << " seconds: ";
            out << entries_amount << std::endl;
            out << "Average % of time the function took over the last ";
            out << std::setprecision(3) << flush.elapsed() << " seconds: ";
            out << std::setprecision(7)
                << avg_over_the_flush * entries_amount / flush.elapsed() * 100;
            out << std::endl;
            out << "Average time the function took over " << entries.size() << " calls: ";
            out << std::setprecision(7) << average * 1000 << " milliseconds" << std::endl;
            out << "Average time the function took over the last " << entries_amount << " calls: ";
            out << std::setprecision(7) << avg_over_the_flush * 1000 << " milliseconds"
                << std::endl;

            spdlog::info(out.str());
        }

        /**
         * @brief Calculate the average execution time of the last N entries.
         * @param last_n_entries Number of last entries to consider.
         * @return The average elapsed time in seconds.
         */
        float avg(const size_t last_n_entries = std::numeric_limits<size_t>::max())
        {
            float rv = 0;
            size_t counter = 0;
            for (int64_t i = static_cast<int64_t>(entries.size()) - 1;
                 counter < last_n_entries && i >= 0; ++counter, --i)
            {
                rv += entries[i].elapsed;
            }
            return rv / counter;
        }

        /**
         * @brief Calculate the average execution time over a specific time interval.
         * @param seconds Time interval in seconds.
         * @return The average elapsed time in seconds.
         */
        float avg_over_the_last(float seconds)
        {
            float rv = 0;
            int64_t i = static_cast<int64_t>(entries.size()) - 1;
            for (; i >= 0; --i)
            {
                rv += entries[i].elapsed;
                if (entries[i].entry_time < entry_time.elapsed() - seconds)
                {
                    break;
                }
            }
            return rv / seconds;
        }

        /**
         * @brief Calculate the average execution time over a specific time interval with a limit.
         * @param seconds Time interval in seconds.
         * @return The average elapsed time in seconds.
         */
        float avg_over_the_last_limited(const float seconds)
        {
            float rv = 0;
            int64_t i = static_cast<int64_t>(entries.size()) - 1;
            for (; i >= 0; --i)
            {
                rv += entries[i].elapsed;
                if (entries[i].entry_time < entry_time.elapsed() - seconds)
                {
                    break;
                }
            }
            if (i < 0 && entries.size() > 0)
            {
                return rv / entries.back().entry_time;
            }
            return rv / seconds;
        }

        /**
         * @brief Calculate the amount of function calls within a specific time interval.
         * @param seconds Time interval in seconds.
         * @return The number of function calls.
         */
        uint64_t amount_of_calls(float seconds)
        {
            uint64_t rv = 0;
            int64_t i = int64_t(entries.size()) - 1;
            for (; i >= 0; --i)
            {
                ++rv;
                if (entries[i].entry_time < entry_time.elapsed() - seconds)
                {
                    break;
                }
            }
            return rv;
        }

        /**
         * @brief Get the total elapsed time since the start of the measurement.
         * @return The elapsed time in seconds.
         */
        float elapsed() { return entry_time.elapsed(); }

        float time_to_flush = 30; ///< Interval in seconds for automatic log flushing.
        size_t maximum_entries =
            std::numeric_limits<size_t>::max(); ///< Maximum number of measurement entries.
        bool log_automatically = true; ///< Determines if measurements are logged automatically.

    private:
        std::string output;         ///< The string identifier for the measurer.
        std::vector<Entry> entries; ///< A vector of measurement entries.
        size_t index = 0;           ///< The current index in the entries vector.
        Timer<clock> flush;         ///< Timer for automatic log flushing.
        Timer<clock> measure;       ///< Timer for measuring execution time.
        Timer<clock> entry_time;    ///< Timer for tracking entry times.
    };
} // namespace utils