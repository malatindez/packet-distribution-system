#pragma once
#include <chrono>
#include <algorithm>
namespace node_system
{
    template<typename ChronoType>
    class ExponentialBackoff {
    public:
        ExponentialBackoff(ChronoType initial_delay, 
                        ChronoType max_delay,
                        double multiplier = 2.0, 
                        double jitter_factor = 0.2)
            : initial_delay_(initial_delay),
            max_delay_(max_delay),
            multiplier_(multiplier),
            jitter_factor_(jitter_factor),
            current_delay_(initial_delay),
            uniform_dist_(0.0, 1.0),
            rng_(std::random_device{}()) {}

        ChronoType get_current_delay() {
            ChronoType jitter = std::chrono::duration_cast<ChronoType>(current_delay_ * jitter_factor_ * uniform_dist_(rng_));
            return current_delay_ + jitter;
        }

        void increase_delay() {
            current_delay_ = std::min(std::chrono::duration_cast<ChronoType>(current_delay_ * multiplier_), max_delay_);
        }

        void reset_delay() {
            current_delay_ = initial_delay_;
        }


    private:
        const ChronoType initial_delay_;
        const ChronoType max_delay_;
        const double multiplier_;
        // Randomizing the backoff delay by a certain percentage can prevent synchronized retries from multiple nodes (the thundering herd problem).
        const double jitter_factor_;
        ChronoType current_delay_;
        std::uniform_real_distribution<double> uniform_dist_;
        std::default_random_engine rng_;
    };

    template<typename ChronoType>
    class LinearBackoff {
    public:
        LinearBackoff(ChronoType initial_delay, 
                        ChronoType max_delay,
                        ChronoType step)
            : initial_delay_(initial_delay),
            max_delay_(max_delay),
            step_(step),
            current_delay_(initial_delay) {}

        ChronoType get_current_delay() {
        }

        void increase_delay() {
            current_delay_ = std::min(current_delay_ + step_, max_delay_);
        }

        void reset_delay() {
            current_delay_ = initial_delay_;
        }

    private:
        const ChronoType initial_delay_;
        const ChronoType max_delay_;
        const ChronoType step_;
        ChronoType current_delay_;
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
