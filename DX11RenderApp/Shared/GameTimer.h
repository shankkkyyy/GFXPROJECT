#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();


	float TotalTime() const;
	float DeltaTime() const;

	/* Initialize the base time and prev time, start timer, call before message loop*/
	void Reset(); 
	void Unpause();
	void Pause();

	/* Call every frame, update deltaTime*/
	void Tick();  

private:

	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedDuration;
	__int64 mPausedTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mPaused;
 
};

