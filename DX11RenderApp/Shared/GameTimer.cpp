#include "GameTimer.h"
#include "windows.h"


GameTimer::GameTimer(): mBaseTime(0), mCurrTime(0), mPrevTime(0), mPausedDuration(0), mPausedTime(0), mPaused(false), mDeltaTime(-1.0f)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0f / (double)countsPerSec;
}


GameTimer::~GameTimer()
{
}

float GameTimer::TotalTime() const
{
	if (mPaused)
	{
		return (float) ((mPausedTime - mPausedDuration - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)((mCurrTime - mPausedDuration - mBaseTime) * mSecondsPerCount);
	}

	return 0.0f;
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;

	mPaused = false;
	mPausedTime = 0;
}

void GameTimer::Unpause()
{
	if (mPaused)
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		// Calculate the paused duration in count
		mPausedDuration += (mPausedTime - startTime);

		// update previous time to unpaused time
		mPrevTime = startTime;

		// no longer paused
		mPaused = false;		
	}
}

void GameTimer::Pause()
{
	if (!mPaused)
	{
		QueryPerformanceCounter((LARGE_INTEGER*) &mPausedTime);

		mPaused = true;
	}
}

void GameTimer::Tick()
{
	if (mPaused)
	{
		mDeltaTime = 0.0f;
		return;
	}
	else
	{
		QueryPerformanceCounter((LARGE_INTEGER*) &mCurrTime);
		
		mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

		mPrevTime = mCurrTime;

		if (mDeltaTime < 0)
		{
			mDeltaTime = 0.0f;
		}
	}

}
