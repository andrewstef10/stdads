#include <gtest/gtest.h>
#include <stdx/stopwatch.h>

#include <stdx/math.h>
#include <thread>

TEST(StopWatchTest, DefaultState) {
    stdx::stopwatch sw;

    EXPECT_FALSE(sw.running());
    EXPECT_DOUBLE_EQ(0.0, sw.get_elapsed_time_sec());
}

TEST(StopWatchTest, StartSetsRunning) {
    stdx::stopwatch sw;

    sw.start();
    EXPECT_TRUE(sw.running());
}

TEST(StopWatchTest, StopSetsNotRunning) {
    stdx::stopwatch sw;

    sw.start();
    sw.stop();
    EXPECT_FALSE(sw.running());
}

TEST(StopWatchTest, StopNotRunningTimerDoesNothing) {
    stdx::stopwatch sw;

    sw.stop();
    EXPECT_FALSE(sw.running());
    EXPECT_DOUBLE_EQ(0.0, sw.get_elapsed_time_sec());
}

TEST(StopWatchTest, ElapsedTimeIncreasesWhileRunning) {
    stdx::stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    double t1 = sw.get_elapsed_time_sec();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    double t2 = sw.get_elapsed_time_sec();

    EXPECT_GT(t1, 0.0);
    EXPECT_GT(t2, t1); // ensure monotonic

    t1 = sw.get_elapsed_time_sec();
    t2 = sw.get_elapsed_time_sec();
    EXPECT_GE(t2, t1); // ensure monotonic
}

TEST(StopWatchTest, ElapsedTimeStopsAfterStop) {
    stdx::stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    sw.stop();

    double t1 = sw.get_elapsed_time_sec();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    double t2 = sw.get_elapsed_time_sec();

    EXPECT_EQ(t1, t2);
}

TEST(StopWatchTest, ResetClearsTime) {
    stdx::stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    sw.stop();

    sw.reset();

    EXPECT_EQ(0.0, sw.get_elapsed_time_sec());
}

TEST(StopWatchTest, StartWithResetClearsTime) {
    stdx::stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    sw.stop();

    sw.start(true); // reset + start

    EXPECT_TRUE(sw.running());
    EXPECT_LT(sw.get_elapsed_time_sec(), 0.01);
}

TEST(StopWatchTest, MultipleStartCallsDoNothingIfAlreadyRunning) {
    stdx::stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    sw.start(true); // should do nothing

    EXPECT_GT(sw.get_elapsed_time_sec(), 0.0);
    EXPECT_TRUE(sw.running());

    long long elapsedTimeMs = sw.get_elapsed_time<std::chrono::milliseconds>().count();
    EXPECT_GT(elapsedTimeMs, 0);
}

TEST(StopWatchTest, MeasuresApproximately100ms) {
    stdx::stopwatch sw;

    std::chrono::steady_clock::time_point realStart = std::chrono::steady_clock::now();
    sw.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::chrono::steady_clock::time_point realEnd = std::chrono::steady_clock::now();
    sw.stop();

    // Compare against actual steady_clock start and end time to combat OS scheduling differences
    double expectedElapsedTimeSec = std::chrono::duration<double>(realEnd - realStart).count();
    double elapsedTimeSec = sw.get_elapsed_time_sec();
    EXPECT_NEAR(expectedElapsedTimeSec, elapsedTimeSec, 0.001); // 1ms tolerance

    long long expectedElapsedTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(realEnd - realStart).count();
    long long elapsedTimeMs = sw.get_elapsed_time<std::chrono::milliseconds>().count();
    EXPECT_TRUE(stdx::abs(expectedElapsedTimeMs - elapsedTimeMs) <= 1LL); // 1ms tolerance
}

TEST(StopWatchTest, CopyConstructorAndAssignmentOperator) {
    stdx::stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    sw.stop();

    // Copy stopped timer
    stdx::stopwatch copy(sw);
    EXPECT_EQ(sw.running(), copy.running());
    EXPECT_EQ(sw.get_elapsed_time_sec(), copy.get_elapsed_time_sec());
    EXPECT_NE(&sw, &copy);

    stdx::stopwatch assignmentCopy;
    assignmentCopy = sw;
    EXPECT_EQ(sw.running(), assignmentCopy.running());
    EXPECT_EQ(sw.get_elapsed_time_sec(), assignmentCopy.get_elapsed_time_sec());
    EXPECT_NE(&sw, &assignmentCopy);

    // Copy running timer
    sw.start();

    stdx::stopwatch copy2(sw);
    EXPECT_EQ(sw.running(), copy2.running());
    EXPECT_NE(&sw, &copy);

    stdx::stopwatch assignmentCopy2;
    assignmentCopy2 = sw;
    EXPECT_EQ(sw.running(), assignmentCopy2.running());
    EXPECT_NE(&sw, &assignmentCopy2);
}
