#ifndef LEG_H
#define LEG_H

#include<cmath>
#include<functional>

class Leg
{
    public:
        float x0, y0;
        float len;
        float acc = 0.0f;
        Leg *prev, *next;
        Leg( float X0, float Y0, float Len );
        virtual ~Leg();

        virtual float x( float s )const = 0;
        virtual float y( float s )const = 0;
        virtual void T( float s, float& Tx, float& Ty )const;// provide unit tangent to curve at s
        virtual const Leg* update( float& rx, float& ry, float& s, float& v, float dt )const;// see mouseLeg
   //     virtual const Leg* update( float& rx, float& ry, float& s, float v, float dt );
        const Leg* updateGrav( float& rx, float& ry, float& s, float& v, float dt, float g )const;

        float V0( float Vf ) const { return sqrtf( Vf*Vf - 2.0f*acc*len ); }// from assigned acc,  len
        float Acc( float V0, float Vf ) { acc = 0.5f*( Vf*Vf - V0*V0 )/len; return acc; }// from assigned  len
};

/*
class linLeg;

class LegAdapter
{
    public:
    const Leg* pLeg = nullptr;// current
    const Leg* pLeg0 = nullptr;// initial
    std::function<void(float,float)> pSetPosition = nullptr;
    float x=0.0f, y=0.0f, s=0.0f, v=0.0f;
 //   float acc = 0.0f;
    float delay = 0.0f;
  //  float v1=0.0f;// v1 = vel on const speed segment
  //  float xa=0.0f, v0=0.0f, ka=0.0f;
  //  float xb=0.0f, vf=0.0f, kb=0.0f;
    float offX = 0.0f, offY = 0.0f;
    void update( float dt );
    void init( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float off_x=0.0f, float off_y=0.0f  );
    void init( linLeg& rLL, std::function<void(float,float)> p_SetPosition, float iPosx, float iPosy, float fPosx, float fPosy, float velFinal, float Acc, float Delay = 0.0f );
    void reset( float V0, float Delay = 0.0f );
    void applyBrake( float k );
  //  float V0( float Vf ) const { if( !pLeg ) return 0.0f; return sqrtf( Vf*Vf - 2.0f*acc*pLeg->len ); }// from assigned acc,  len
  //  float Acc( float V0, float Vf ) { if( !pLeg ) return 0.0f; acc = 0.5f*( Vf*Vf - V0*V0 )/pLeg->len; return acc; }// from assigned  len
  //  void init_ab( float Xa, float V0, float Xb, float Vf );// defaults disable ab segment motion
};
*/

#endif // LEG_H
