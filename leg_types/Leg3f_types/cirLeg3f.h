#ifndef CIRLEG3F_H
#define CIRLEG3F_H

#include "Leg3f.h"


class cirLeg3f : public Leg3f
{
    public:
    const float PI = 3.141593f;
    vec3f ctr;// arc center
    vec3f T0;// given tangent at s=0 and part of vector basis
    vec3f Nu;// given tangent at s=0 and part of vector basis
    vec3f Tc;// points from pos0 to ctr. Tc = Nu.cross( T0 )
    float R = 1.0f;
    float angle = 0.0f;// limit
    virtual vec3f getPos( float s )const;
    virtual vec3f T( float s )const;// provide unit tangent to curve at s
    vec3f rHat( float s )const;// points away from ctr
    virtual vec3f accel( float s, float v )const { return -rHat(s)*(v*v/R); }// centripetal

    cirLeg3f();

    // Dir = 'L' = left, 'R', 'U' and 'D' used to assign nu then call init() 3rd below
     void init( vec3f Pos0, vec3f t0, char Dir, float r, float Angle );// 1st leg - calls other 1st Leg init()
     cirLeg3f( vec3f Pos0, vec3f t0, char Dir, float r, float Angle ){ init( Pos0, t0, Dir, r, Angle ); }
     void init( Leg3f& rPrev, char Dir, float r, float Angle );// mid legs
    cirLeg3f( Leg3f& rPrev, char Dir, float r, float Angle  ){ init( rPrev, Dir, r, Angle ); }// mid legs

     void init( vec3f Pos0, vec3f t0, vec3f nu, float r, float Angle );// 1st leg
    cirLeg3f( vec3f Pos0, vec3f t0, vec3f nu, float r, float Angle ){ init( Pos0, t0, nu, r, Angle ); }

    void init( Leg3f& rPrev, vec3f nu, float r, float Angle );// mid legs
    cirLeg3f( Leg3f& rPrev, vec3f nu, float r, float Angle  ){ init( rPrev, nu, r, Angle ); }// mid legs

    void init( Leg3f& rPrev, Leg3f& rNext, vec3f nu, float r, float Angle  );// last (tie) leg
    cirLeg3f( Leg3f& rPrev, Leg3f& rNext, vec3f nu, float r, float Angle  ){ init( rPrev, rNext, nu, r, Angle  ); }

    void init( vec3f Pos0 , Leg3f& rNext, vec3f t0, vec3f nu, float r, float Angle );// entry leg
    cirLeg3f( vec3f Pos0 , Leg3f& rNext, vec3f t0, vec3f nu, float r, float Angle  ){ init( Pos0 ,rNext, t0, nu, r, Angle  ); }

    virtual ~cirLeg3f(){}
};

#endif // CIRLEG3F_H
