#include "metronome.h"
#include <iostream>
#include <chrono>

std::chrono::time_point<std::chrono::steady_clock> last;
Metronome::Metronome(){
     last = std::chrono::steady_clock::now();
}


void Metronome::StartMetronome(int beats_per_minute){
    duration_ = std::chrono::milliseconds(beats_per_minute);
    //Start thread
    tick_thread_ = new std::thread(&Metronome::TickThread, this);
    callback_thread_ = new std::thread(&Metronome::CallBackThread, this);
}

void Metronome::AddCallback(std::function<void(int)> fn){
    callbacks_.push_back(fn);
}


void tick(int beat){
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
    last = now;
    std::cout << "GOT BEAT: " << beat << " @ " << ms.count() << std::endl;
}
void Metronome::CallBackThread(){
    int beat = -1;
    auto last = std::chrono::steady_clock::now();
    while(true){
        {
            //Wait for our next beat!
            std::unique_lock<std::mutex> lock(mutex_);
            ticked_condition_.wait(lock);
            beat = beat_count;
        }
        auto now = std::chrono::steady_clock::now();
        
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
        last = now;
        std::cout << "GOT BEAT: " << beat << " @ " << ms.count() << std::endl;
        for(auto fn : callbacks_){
            fn(beat);
        }
    }
}

void Metronome::TickThread(){
    std::chrono::microseconds desired_duration = duration_;
    std::chrono::microseconds actual_duration = duration_;
    while(true){
        //std::cout << "Actual Duration: " << actual_duration.count() << std::endl;
        auto start = std::chrono::steady_clock::now();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            this->lock_condition_.wait_for(lock, actual_duration);
        }
        {
            std::unique_lock<std::mutex> lock(mutex_);
            beat_count ++;
            ticked_condition_.notify_all();
        }
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        actual_duration = std::chrono::microseconds(desired_duration.count() + desired_duration.count() - ms.count());
        
        //actual_duration = ms - duration_;
    }
}