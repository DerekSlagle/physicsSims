#ifndef FUNCLEG_H_INCLUDED
#define FUNCLEG_H_INCLUDED

#include <functional>
#include <vector>
#include <iostream>
#include "Leg.h"

class funcLeg : public Leg
{
    public:
    // one pair == nullptr.
    std::function<float(float)> pYx = nullptr, pYPx = nullptr;// cartesian funcs
    std::function<float(float)> pRa = nullptr, pRPa = nullptr;// polar funcs

    std::vector<float> sVec, parVec;
    size_t numSegs = 100;// table build and interpolation intervals = sizeof above vectors

    void init( float X0, float Y0, float par0, float parF, char type, std::function<float(float)> pUv, std::function<float(float)> pUPv, size_t NumSegs = 100 );// type: c=cart, p=polar
    funcLeg( float X0, float Y0, float par0, float parF, char type, std::function<float(float)> pUv, std::function<float(float)> pUPv, size_t NumSegs = 100 ): Leg(X0,Y0,0.0f)
    { init( X0, Y0, par0, parF, type, pUv, pUPv, NumSegs ); }
    // PREV lEG ASSIGNED
    void init( Leg& rLeg, float par0, float parF, char type, std::function<float(float)> pUv, std::function<float(float)> pUPv, size_t NumSegs = 100 );// type: c=cart, p=polar
    funcLeg( Leg& rLeg, float par0, float parF, char type, std::function<float(float)> pUv, std::function<float(float)> pUPv, size_t NumSegs = 100 ): Leg(0.0f,0.0f,0.0f)
    { init( rLeg, par0, parF, type, pUv, pUPv, NumSegs ); }

    virtual float x( float s )const;
    virtual float y( float s )const;
    virtual void set_xy( float s, float& rx, float& ry ) const;

    float find_par( float s )const;
};


#endif // FUNCLEG_H_INCLUDED
