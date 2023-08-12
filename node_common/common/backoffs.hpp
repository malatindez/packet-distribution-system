#pragma once
#include <chrono>
#include <algorithm>
namespace node_system
{
    /**
     * @brief A utility class for implementing exponential backoff strategies.
     *
     * Exponential backoff is a strategy where the delay between retries increases exponentially.
     * This class provides functionality to manage and manipulate the backoff delay.
     * @tparam ChronoType The type representing the time duration (e.g., std::chrono::milliseconds).
     */
    template <typename ChronoType>
    class ExponentialBackoff
    {
    public:
        /**
         * @brief Constructor to initialize the exponential backoff strategy.
         *
         * @param initial_delay The initial delay before the first retry.
         * @param max_delay The maximum delay allowed between retries.
         * @param multiplier The factor by which the delay is multiplied after each retry.
         * @param divisor The factor by which the delay is divided when decreasing the delay.
         * @param jitter_factor The factor by which jitter is applied to the delay to avoid synchronized retries.
         */
        ExponentialBackoff(ChronoType initial_delay,
                           ChronoType max_delay,
                           double multiplier = 2.0,
                           double divisor = 2.0,
                           double jitter_factor = 0.2)
            : initial_delay_(initial_delay),
              max_delay_(max_delay),
              multiplier_(multiplier),
              divisor_(divisor),
              jitter_factor_(jitter_factor),
              current_delay_(initial_delay),
              uniform_dist_(0.0, 1.0),
              rng_(std::random_device{}()) {}

        /**
         * @brief Get the current backoff delay.
         *
         * @return The current backoff delay with applied jitter.
         */
        constexpr ChronoType get_current_delay() noexcept
        {
            ChronoType jitter = std::chrono::duration_cast<ChronoType>(current_delay_ * jitter_factor_ * uniform_dist_(rng_));
            return current_delay_ + jitter;
        }

        /**
         * @brief Increase the backoff delay using exponential factor.
         */
        constexpr void increase_delay() noexcept
        {
            current_delay_ = std::min(std::chrono::duration_cast<ChronoType>(current_delay_ * multiplier_), max_delay_);
        }

        /**
         * @brief Decrease the backoff delay using divisor factor.
         */
        constexpr void decrease_delay() noexcept
        {
            current_delay_ = std::max(std::chrono::duration_cast<ChronoType>(current_delay_ / divisor_), initial_delay_);
        }

        /**
         * @brief Reset the backoff delay to its initial value.
         */
        constexpr void reset_delay() noexcept
        {
            current_delay_ = initial_delay_;
        }

    private:
        const ChronoType initial_delay_;                        ///< The initial delay before the first retry.
        const ChronoType max_delay_;                            ///< The maximum delay allowed between retries.
        const double multiplier_;                               ///< The factor by which the delay is multiplied after each retry.
        const double divisor_;                                  ///< The factor by which the delay is divided when decreasing the delay.
        
        const double jitter_factor_;                            ///< The factor by which jitter is applied to the delay to avoid synchronized retries.
        std::uniform_real_distribution<double> uniform_dist_;   ///< Uniform distribution for generating jitter.
        
        ChronoType current_delay_;                              ///< The current backoff delay.
        std::default_random_engine rng_;                        ///< Random number generator engine.

    };
    
    /**
     * @brief A utility class for implementing linear backoff strategies.
     * 
     * Linear backoff is a strategy where the delay between retries increases linearly.
     * This class provides functionality to manage and manipulate the backoff delay.
     * @tparam ChronoType The type representing the time duration (e.g., std::chrono::milliseconds).
     */
    template <typename ChronoType>
    class LinearBackoff
    {
    public:
        /**
         * @brief Constructor to initialize the linear backoff strategy.
         * 
         * @param initial_delay The initial delay before the first retry.
         * @param max_delay The maximum delay allowed between retries.
         * @param step The increment step used to increase or decrease the delay.
         */
        constexpr LinearBackoff(ChronoType initial_delay,
                                ChronoType max_delay,
                                ChronoType step)
            : initial_delay_(initial_delay),
              max_delay_(max_delay),
              step_(step),
              current_delay_(initial_delay) {}

        /**
         * @brief Get the current backoff delay.
         * 
         * @return The current backoff delay.
         */
        constexpr ChronoType get_current_delay() const noexcept
        {
            return current_delay_;
        }

        /**
         * @brief Increase the backoff delay using the specified step.
         */
        constexpr void increase_delay() noexcept
        {
            current_delay_ = std::min(current_delay_ + step_, max_delay_);
        }

        /**
         * @brief Decrease the backoff delay using the specified step.
         */
        constexpr void decrease_delay() noexcept
        {
            current_delay_ = std::max(current_delay_ - step_, initial_delay_);
        }

        /**
         * @brief Reset the backoff delay to its initial value.
         */
        constexpr void reset_delay() noexcept
        {
            current_delay_ = initial_delay_;
        }

    private:
        const ChronoType initial_delay_;    ///< The initial delay before the first retry.
        const ChronoType max_delay_;        ///< The maximum delay allowed between retries.
        const ChronoType step_;             ///< The increment step used to increase or decrease the delay.
        ChronoType current_delay_;          ///< The current backoff delay.
    };

    using ExponentialBackoffNs = ExponentialBackoff<std::chrono::nanoseconds>;
    using ExponentialBackoffUs = ExponentialBackoff<std::chrono::microseconds>;
    using ExponentialBackoffMs = ExponentialBackoff<std::chrono::milliseconds>;
    using ExponentialBackoffSec = ExponentialBackoff<std::chrono::seconds>;

    using LinearBackoffNs = LinearBackoff<std::chrono::nanoseconds>;
    using LinearBackoffUs = LinearBackoff<std::chrono::microseconds>;
    using LinearBackoffMs = LinearBackoff<std::chrono::milliseconds>;
    using LinearBackoffSec = LinearBackoff<std::chrono::seconds>;

}
