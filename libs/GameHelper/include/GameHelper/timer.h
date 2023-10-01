#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace gamehelper {

class Timer {
public:
    Timer();
    void Update();
    // return previous frame time in milliseconds
    long long FrameElapsed();
private:

#ifdef _MSC_VER
    using timeDuration = std::chrono::steady_clock::time_point; 
#else
    using timeDuration = std::chrono::time_point<
	std::chrono::_V2::system_clock,
	std::chrono::duration<long int, std::ratio<1, 1000000000>>>;
#endif

    timeDuration start;
    timeDuration lastUpdate;
    timeDuration currentUpdate;
    long long frameElapsed = 0;
};

}

#endif
