#ifndef BUTTAUTOREPEAT_H_INCLUDED
#define BUTTAUTOREPEAT_H_INCLUDED

#include "buttonRect.h"

class buttAutoRepeat : public buttonRect
{
    public:
    static int pauseCount, stepCount;
    int pauseDelay = 30, stepDelay = 4;

    void init( float x, float y, float W, float H, std::function<void(void)> p_HitFunc, const char* label, int PauseDelay = 30, int StepDelay = 4 );
    virtual bool hitLeft();// zero the counts
    virtual void update();// iterate counts and call pHitFunc()
};

#endif // BUTTAUTOREPEAT_H_INCLUDED
