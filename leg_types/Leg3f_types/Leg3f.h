#ifndef LEG3F_H
#define LEG3F_H

#include<cmath>
#include<functional>
#include<iostream>
#include "../../vec2f.h"

class Leg3f
{
    public:
        vec3f pos0;// position when s = 0.0f
        vec3f posF;// position when s = len. Cached value
        float len;
        Leg3f *prev, *next;
        Leg3f( vec3f Pos0, float Len );
        Leg3f();
        virtual ~Leg3f(){}

        float x( float s )const { return getPos(s).x; }
        float y( float s )const { return getPos(s).y; }
        float z( float s )const { return getPos(s).z; }

        virtual vec3f getPos( float s )const = 0;
        virtual vec3f T( float s )const = 0;// provide unit tangent to curve at s
        virtual vec3f accel( float s, float v )const = 0;

        const Leg3f* update( vec3f& pos, float& s, float v, float dt )const;
        static void destroy3fPath( Leg3f*& pPath );// for dynamically allocated legs owned by this
   //     const Leg* updateGrav( vec3f& pos, float& s, float& v, float dt, float g )const;
};

class Leg3f_adapter
{
public:

    float s = 0.0f;// position parameter
    float v = 0.0f;// speed along Leg
    const Leg3f* pLeg0 = nullptr;// const applies to object pointed to. pLeg0 can be assigned.D
    const Leg3f* pLegCurr = nullptr;// is updated by retVal from Leg::update()
    vec3f *pVel = nullptr, *pAccel = nullptr, *pTu = nullptr;// write in update()
    std::function<void(vec3f)> pSetPosition = nullptr;
    void update( float dt );
    vec3f accel()const;
    vec3f velocity()const { return pLegCurr ? v*pLegCurr->T(s) : vec3f(); }

    Leg3f_adapter(){}
    void init( const Leg3f& rLeg0, std::function<void(vec3f)> p_SetPosition );
    void reset();
    Leg3f_adapter( const Leg3f& rLeg0, std::function<void(vec3f)> p_SetPosition ){ init( rLeg0, p_SetPosition ); }
};

#endif // LEG3F_H
