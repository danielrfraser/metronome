#ifndef METRONOME_H
#define METRONOME_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <vector>

class Metronome{
    public:
        Metronome();
        void StartMetronome(int beats_per_minute);
        void AddCallback(std::function<void(int)> fn);
    private:
        void CallBackThread();
        void TickThread();
        std::thread* tick_thread_;
        std::thread* callback_thread_;
        
        std::mutex ticked_mutex_;
        std::condition_variable ticked_condition_;
        int beat_count = 0;

        std::vector<std::function<void(int)> > callbacks_;
        std::chrono::milliseconds duration_;
        std::chrono::microseconds tick_duration_;

        std::mutex mutex_;
        std::condition_variable lock_condition_;
};

#endif //METRONOME_H