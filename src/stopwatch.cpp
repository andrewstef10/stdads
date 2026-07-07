#include <stdx/stopwatch.h>

namespace stdx {

stopwatch::stopwatch() : m_running(false) { reset(); }

void stopwatch::start(bool resetStopwatch) {
    if (!m_running) {
        if (resetStopwatch) {
            reset();
        } else {
            m_startTime = now();
        }
        m_running = true;
    }
}

void stopwatch::stop() {
    if (m_running) {
        m_stopTime = now();
        m_running = false;
    }
}

void stopwatch::reset() {
    m_startTime = now();
    m_stopTime = m_startTime;
}

double stopwatch::get_elapsed_time_sec() const {
    return get_elapsed_time<std::chrono::duration<double, std::chrono::seconds::period>>().count();
}
} // namespace stdx
