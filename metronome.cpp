#include "metronome.h"
#include <iostream>
#include <chrono>

std::chrono::time_point<std::chrono::steady_clock> last;
Metronome::Metronome(){
     last = std::chrono::steady_clock::now();
}


void Metronome::StartMetronome(int beats_per_minute){
    //Go to 5 times as long 
    duration_ = std::chrono::milliseconds(beats_per_minute);
    tick_duration_ = std::chrono::microseconds(beats_per_minute * 1000);
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
    //std::cout << "GOT BEAT: " << beat << " @ " << ms.count() << std::endl;
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
        //std::cout << "GOT BEAT: " << beat << " @ " << ms.count() << std::endl;
        for(auto fn : callbacks_){
            fn(beat);
        }
    }
}

void Metronome::TickThread(){
    std::chrono::microseconds desired_duration = duration_;
    std::chrono::microseconds actual_duration = duration_;
    while(true){
        auto current_time = std::chrono::steady_clock::now();
        
        auto next_tick = current_time + duration_;
         auto close_time = next_tick - std::chrono::microseconds(1000);

        //Work in groups of two
        auto c_tick = tick_duration_ / 2000;

        int count = 0;
        int tick_d = 0;

     
            
        
       // do{
            
           /* std::unique_lock<std::mutex> lock(mutex_);
            this->lock_condition_.wait_for(lock, c_tick);

            current_time = std::chrono::steady_clock::now();
            auto delta = next_tick - current_time;
            */
            //If the time is less than our sleep time
            //if(delta < c_tick){
            while(std::chrono::steady_clock::now() < close_time){
                std::unique_lock<std::mutex> lock(mutex_);
                this->lock_condition_.wait_for(lock, c_tick);
                count ++;

            }

           
            //}
            //current_time = std::chrono::steady_clock::now();
            
        //}while(current_time < next_tick);

        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(std::chrono::steady_clock::now() < next_tick){
                tick_d ++;
            }
            beat_count ++;
            ticked_condition_.notify_all();
        }
       /// std::cout << "count: " << count << " Delta: " << tick_d << std::endl;
    }
}