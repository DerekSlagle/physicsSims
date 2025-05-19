#ifndef LEGADAPTER_H_INCLUDED
#define LEGADAPTER_H_INCLUDED

#include "Leg.h"
#include<iostream>

class linLeg;

class legAdapter
{
    public:
    const Leg* pLeg = nullptr;// current
    const Leg* pLeg0 = nullptr;// initial
    std::function<void(float,float)> pSetPosition = nullptr;
    float x=0.0f, y=0.0f, s=0.0f, v=0.0f;// on Leg
    float delay = 0.0f;
    float offX = 0.0f, offY = 0.0f;
    legAdapter* pBaseLA = nullptr;
    virtual void update( float dt );
    virtual void setPosition( float X, float Y ) { x = X; y = Y; if( pSetPosition ) pSetPosition(X,Y); }
    virtual void getPosition( float& X, float& Y ) const { X = x; Y = y; }
    void init( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float off_x=0.0f, float off_y=0.0f  );
    void init( linLeg& rLL, std::function<void(float,float)> p_SetPosition, float iPosx, float iPosy, float fPosx, float fPosy, float velFinal, float Acc, float Delay = 0.0f );
    virtual void reset( float V0, float Delay = 0.0f );
    void reset( float Delay = 0.0f ){ reset( v, Delay ); }
    void applyBrake( float k );

    legAdapter(){}
    legAdapter( Leg* p_Leg, std::function<void(float,float)> p_SetPosition, float vel, float Delay=0.0f, float off_x=0.0f, float off_y=0.0f ): delay(Delay)
    { init( p_Leg, p_SetPosition, vel, off_x, off_y ); }
};

class inertialAdapter : public legAdapter
{
    public:
    float x1=0.0f, y1=0.0f, v1x=0.0f, v1y=0.0f;// position and velocity of the positioned object
    float m = 1.0f, Cs = 3.0f, Cd = 3.0f;// inertial
    float R = 1.0f, Cr = 0.8f;// collision handling: radius and coeff. of restitution

    void initInertial( float cs, float cd, float radius, float M = 1.0f ){ Cs = cs; Cd = cd; R = radius; m = M; }
    virtual void update( float dt );
    void update( float dt, float accx, float accy );// apply external acceleration
    virtual void setPosition( float X, float Y ) { x1 = X; y1 = Y; if( pSetPosition ) pSetPosition(X,Y); }
    virtual void getPosition( float& X, float& Y ) const { X = x1; Y = y1; }
    virtual void reset( float V0, float Delay = 0.0f );
    virtual void onLaunch();// sync motion with pBaseLA

    bool hitRigid( float hitX, float hitY, float hitR );// collision response with fixed object
    bool hitFree( float& hitX, float& hitY, float& hitVx, float& hitVy, float hitR, float hitMass );// collision response with momentum conserved. Classic 2 body
    bool hitFreeLine( float& hitX, float& hitY, float& hitVx, float& hitVy, float hitR, float hitMass );// ray algo great for high speed
    bool keepInBox( float left, float right, float top, float bottom );// handle collisions with inside of box
};

class impactTarget : public inertialAdapter
{
    public:
    float health = 5.0f;
    static float FceToDmgGrad, dVelMin;// least speed change to cause damage
    float vLastX = 0.0f, vLastY = 0.0f;
    std::function<void(float)> pSetHealth = nullptr;

    virtual void update( float dt );// find damege here
    virtual void reset( float V0, float Delay = 0.0f );// for vLast
    virtual void onLaunch();// sync motion with pBaseLA
    bool keepInBox_soft( float left, float right, float top, float bottom );// handle collisions with inside of box
};

class freeMover
{
    public:
    float x=0.0f, y=0.0f;
    float vx=0.0f, vy=0.0f;
    float m=1.0f, R=1.0f, Cr = 0.8f;
    float tLife = 0.0f;// lifetime
    std::function<void(float,float)> pSetPosition = nullptr;

    void update( float dt ){ if( tLife <= 0.0f ) return; tLife -= dt; if( tLife > 0.0f ){ x += vx; y += vy; if( pSetPosition ) pSetPosition(x,y); } }
    void setPosition( float X, float Y ) { x = X; y = Y; if( pSetPosition ) pSetPosition(X,Y); }
    void init( float X, float Y, float Vx, float Vy, float Mass, float Rad, float t_Life, std::function<void(float,float)> p_SetPosition )
    { x = X; y = Y; vx = Vx; vy = Vy; m = Mass; R = Rad; tLife = t_Life; pSetPosition = p_SetPosition; }
    void reset( float X, float Y, float Vx, float Vy, float t_Life ) { x = X; y = Y; vx = Vx; vy = Vy; tLife = t_Life; }
    bool hitFreeLine( float hitX, float hitY, float& hitVx, float& hitVy, float hitR, float hitMass );
};


#endif // LEGADAPTER_H_INCLUDED
