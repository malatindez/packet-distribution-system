#pragma once
namespace utils
{
    /**
     * @brief A simple timer class for measuring elapsed time using various clock types.
     *
     * This class provides the ability to measure elapsed time using different clock types
     * (e.g., steady_clock, high_resolution_clock).
     */
    template <class clock> class Timer
    {
    public:
        /**
         * @brief Constructs a Timer and initializes it with the current time.
         */
        Timer()
        {
            current_ = clock::now();
            reset();
        }

        /**
         * @brief Resets the start time to the current time.
         */
        void reset_to_now() { start_ = clock::now(); }

        /**
         * @brief Resets the start time to the previously captured current time.
         */
        void reset() { start_ = current_; }

        /**
         * @brief Calculates and returns the elapsed time since the timer was last reset.
         *
         * @return The elapsed time in seconds.
         */
        constexpr float elapsed() noexcept
        {
            current_ = clock::now();
            return std::chrono::duration_cast<std::chrono::duration<float>>(current_ - start_)
                .count();
        }

    private:
        std::chrono::time_point<clock> start_;   ///< The time point representing the start time.
        std::chrono::time_point<clock> current_; ///< The time point representing the current time.
    };

    /**
     * @brief Alias for Timer using std::chrono::steady_clock as the clock type.
     */
    using SteadyTimer = Timer<std::chrono::steady_clock>;

    /**
     * @brief Alias for Timer using std::chrono::high_resolution_clock as the clock type.
     */
    using HighResolutionTimer = Timer<std::chrono::high_resolution_clock>;

} // namespace utils