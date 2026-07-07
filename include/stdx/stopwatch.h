#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

namespace stdx {

class stopwatch {
  public:
    /**
     * @brief Default constructor.
     *
     * Creates a stopwatch that is not running with no elapsed time.
     */
    stopwatch();

    /**
     * @brief Default Destructor.
     */
    ~stopwatch() = default;

    /**
     * @brief Copy constructor.
     *
     * @param
     */
    stopwatch(const stopwatch&) = default;

    /**
     * @brief Assignment operator.
     *
     * @param
     * @return Reference to this object
     */
    stopwatch& operator=(const stopwatch&) = default;

    /// @brief Move constructor
    /// @param
    stopwatch(stopwatch&&) = default;

    /// @brief Move assignment operator
    /// @param
    /// @return Reference to this object
    stopwatch& operator=(stopwatch&&) = default;

    /**
     * @brief Starts the stopwatch.
     *
     * If the stopwatch is already running, this function does nothing.
     *
     * @param resetStopwatch True to also reset the time, otherwise (default) false to not reset.
     */
    void start(bool resetStopwatch = false);

    /**
     * @brief Stops the stopwatch.
     */
    void stop();

    /**
     * @brief Resets the stopwatch.
     */
    void reset();

    /**
     * @brief Get the elapsed time.
     *
     * @return Elapsed time in the desired units as a duration
     */
    template <typename Duration>
    Duration get_elapsed_time() const;

    /**
     * @brief Get the elapsed time in seconds.
     *
     * @return Elapsed time in seconds
     */
    double get_elapsed_time_sec() const;

    /**
     * @brief Get if the stopwatch is running.
     *
     * @return True if the stopwatch is running, false otherwise.
     */
    bool running() const { return m_running; }

  private:
    /**
     * @brief Helper function to get the current steady time.
     *
     * @return time_point with the current steady time using chrono
     */
    static std::chrono::steady_clock::time_point now() { return std::chrono::steady_clock::now(); }

    std::chrono::steady_clock::time_point m_startTime;
    std::chrono::steady_clock::time_point m_stopTime;
    bool m_running;
};

template <typename Duration>
inline Duration stopwatch::get_elapsed_time() const {
    std::chrono::steady_clock::time_point latestTime = m_running ? now() : m_stopTime;
    return std::chrono::duration_cast<Duration>(latestTime - m_startTime);
}
} // namespace stdx

#endif
