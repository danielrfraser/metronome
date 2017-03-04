#include <signal.h>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include<windows.h>


#include "metronome.h"

std::condition_variable lock_condition_;
std::mutex mutex_;

void signal_handler (int signal_value){
	//Gain the lock so we can notify to terminate
	std::unique_lock<std::mutex> lock(mutex_);
	lock_condition_.notify_all();
}

void test_callback(int beat_number){
    //std::cout << "Beat: \a" << beat_number << std::endl;
    if(beat_number % 4 == 0){
        Beep(1245, 50);
    }else{
        Beep(1568, 50);
    }
}



int main(){
    //Handle the SIGINT/SIGTERM signal
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
    
    Metronome* m = new Metronome();
    m->StartMetronome(500);
    m->AddCallback(&test_callback);

    {
		std::unique_lock<std::mutex> lock(mutex_);
		//Wait for the signal_handler to notify for exit
		lock_condition_.wait(lock);
	}

    return 0;
}