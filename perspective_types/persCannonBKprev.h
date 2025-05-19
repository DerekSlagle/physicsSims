#ifndef PERSCANNON_H
#define PERSCANNON_H

#include "persPt.h"


class persCannon : public persPt
{
    public:
    std::function<bool( vec3f, vec3f, bool )> pFireShot = nullptr;
    std::function<bool( vec3f )> pGetFireball = nullptr;
    persBall Base;// A round image
 //   vec3f firePos;
    vec3f shotVel;// assigned in getFiringVelocity
    float tFlight = 0.0f;// as above
    float shotSpeed = 3000.0f;
    float gravY = -200.0f;
    float barrelLength = 200.0f;
    bool aimingHigh = false;
    bool firingEnabled = false;// respond to fire signals
    bool prevTriggerState = false;
    bool *pTrigger = nullptr;// trigger is being held closed, to fire...
    vec3f AimPt;// target position, updated by caller
    float dtFire = 0.2f;// 5 rounds/second
    float tElap = 0.0f;
    bool getFiringVelocity();

    // animated movement
    vec3f movePt;
    float moveSpeed = 400.0f;
    void startMove( vec3f movePos );// trigger

    void init( std::istream& is, bool& rTrigger, std::function<bool( vec3f, vec3f, bool )> p_FireShot );
    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    persCannon(){}
    virtual ~persCannon(){}
};

#endif // PERSCANNON_H
