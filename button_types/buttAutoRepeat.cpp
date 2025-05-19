#include "buttAutoRepeat.h"

int buttAutoRepeat::pauseCount = 0, buttAutoRepeat::stepCount = 0;

void buttAutoRepeat::init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const char* label, int PauseDelay, int StepDelay )
{
    pauseDelay = PauseDelay;
    stepDelay = StepDelay;
    buttonRect::init( x, y, W, H, p_HitFunc, label );
    mode = 'I';
}

bool buttAutoRepeat::hitLeft()// zero the counts
{
    buttonRect::hitLeft();
    if( sel ) pauseCount = stepCount = 0;
    return mseOver;
}

void buttAutoRepeat::update()// iterate counts and call pHitFunc()
{
    if( !( sel && pHitFunc ) ) return;
    if( pauseCount < pauseDelay ) { ++pauseCount; return; }
    if( stepCount < stepDelay ) { ++stepCount; return; }

    pHitFunc();
    stepCount = 0;
}
