#pragma once
#include <functional>

namespace utils
{
    /**
     * @brief Get the address of a callable function.
     *
     * @tparam T Return type of the function.
     * @tparam U Argument types of the function.
     * @param f The function to get the address of.
     * @return size_t The address of the function.
     */
    template <typename T, typename... U> size_t GetFunctionAddress(std::function<T(U...)> f)
    {
        typedef T(FnType)(U...);
        FnType **fnPointer = f.template target<FnType *>();
        return reinterpret_cast<size_t>(*fnPointer);
    }
    // TODO (malatindez): improve this later
    // add support for class methods

    /**
     * @brief A system for managing and invoking callback functions.
     *
     * @tparam F Type of the callback function.
     */

    template <typename F> class CallbackSystem
    {
    public:
        using CallbackFunction = F;
        using CallbackId = uint32_t;
        using CallbackAddress = size_t;

        /**
         * @brief Add a new callback function.
         *
         * @param callback The callback function to add.
         * @return CallbackId The ID of the added callback.
         */
        CallbackId AddCallback(CallbackFunction const &callback)
        {
            auto id = next_id_++;
            callbacks_.emplace(id, std::move(callback));
            callback_addresses_.emplace(id, GetFunctionAddress(callback));
            return id;
        }

        /**
         * @brief Remove a callback by its ID.
         *
         * @param id The ID of the callback to remove.
         */
        void RemoveCallback(CallbackId id)
        {
            callbacks_.erase(id);
            callback_addresses_.erase(id);
        }

        /**
         * @brief Remove a callback by its function object.
         *
         * @param callback The callback function to remove.
         */
        void RemoveCallback(CallbackFunction const &callback)
        {
            auto address = GetFunctionAddress(callback);
            for (auto const &[id, callback_address] : callback_addresses_)
            {
                if (callback_address == address)
                {
                    RemoveCallback(id);
                    return;
                }
            }
        }

        /**
         * @brief Invoke all registered callbacks with the provided data.
         *
         * @param data The data to be passed to the callbacks.
         */
        void InvokeCallbacks(T const &data)
        {
            for (auto const &[id, callback] : callbacks_)
            {
                callback(data);
            }
        }

    private:
        std::unordered_map<CallbackId, Callback> callbacks_;
        std::unordered_map<CallbackId, CallbackAddress> callback_addresses_;
        CallbackId next_id_ = 0;
    };
} // namespace utils
