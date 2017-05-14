#include <signal.h>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include<windows.h>
#include <fstream> 
#include <chrono>
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

bool got_last = false;
std::chrono::time_point<std::chrono::steady_clock> last2;
std::fstream fs;
void test_callback2(int beat_number){
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - last2);
    last2 = now;
    beat_number --;
    beat_number = beat_number % 4;
    auto output = new char[2 + (3 * 4) + 1];

    output[0] = '[';
    output[1 + (3*4)] = ']';
    for(int i = 1; i < 1 + (3 * 4); i++){
        output[i] = ' ';
    }
    output[2 + (3 * 4)] = NULL;
    //std::cout << "beat_number: " << beat_number << std::endl;
    output[1 + beat_number * 3 + 1] = '1' + beat_number;

    std::cout << "\r" << output;
    
    if(got_last){
        std::cout << ' ' << ms.count() << "uS";
    }else{
        got_last = true;
    }
    delete output;

    last2 = std::chrono::steady_clock::now();

}


int main(){
    //Handle the SIGINT/SIGTERM signal
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
    
   //fs.open ("test.txt", std::fstream::in | std::fstream::out | std::fstream::app);

    Metronome* m = new Metronome();
    m->StartMetronome(500);
    m->AddCallback(&test_callback2);
    m->AddCallback(&test_callback);
    

    {
		std::unique_lock<std::mutex> lock(mutex_);
		//Wait for the signal_handler to notify for exit
		lock_condition_.wait(lock);
	}

     

   // fs.close();

    return 0;
}