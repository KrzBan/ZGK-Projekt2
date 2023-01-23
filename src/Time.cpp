#include "Time.hpp"

struct TimeData {
	double time = 0.0;
	double deltaTime = 0.0;
};

static TimeData timeData{};

double Time::GetDeltaTime() {
	return timeData.deltaTime;
}
double Time::GetTime() {
	return timeData.time;
}

void Time::UpdateTime(double time) {
	timeData.deltaTime = time - timeData.time;
	timeData.time = time;
}