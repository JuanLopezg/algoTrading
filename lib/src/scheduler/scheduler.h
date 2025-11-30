#pragma once
#include <thread>
#include <atomic>
#include <chrono>

template<typename TContext>
class Scheduler {
public:
    Scheduler(TContext& ctx,
              std::chrono::seconds interval,
              std::chrono::seconds timeout)
        : ctx(ctx),
          interval(interval),
          timeout(timeout)
    {}

    virtual ~Scheduler() {
        stop();
    }

    void start() {
        running = true;
        worker = std::thread([this]() { loop(); });
    }

    void stop() {
        running = false;
        if (worker.joinable())
            worker.join();
    }

protected:
    virtual void processSecond(TContext& ctx) = 0;

private:
    void loop() {
        auto last = std::chrono::steady_clock::now();

        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto now = std::chrono::steady_clock::now();

            if (now - last >= interval) {
                processSecond(ctx);
                last = now;
            }
        }
    }

    TContext& ctx;
    std::chrono::seconds interval;
    std::chrono::seconds timeout;

    std::atomic<bool> running{false};
    std::thread worker;
};
