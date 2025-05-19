#ifndef PERSCANNON_H
#define PERSCANNON_H

#include "persPt.h"


class persCannon : public persPt
{
    public:
    std::function<bool( vec3f, vec3f, bool )> pFireShot = nullptr;
    std::function<bool( vec3f )> pGetFireball = nullptr;
    sf::CircleShape baseImg;
    sf::Vertex vtx[4];// the barrel
    vec3f shotVel;// assigned in getFiringVelocity
    float tFlight = 0.0f;// as above
    float shotSpeed = 3000.0f;
    float gravY = -200.0f;
    float barrelLength = 200.0f, barrelWidth = 20.0f;
    bool aimingHigh = false;
    bool firingEnabled = false;// respond to fire signals
    bool prevTriggerState = false;
    bool *pTrigger = nullptr;// trigger is being held closed, to fire...

    vec3f AimPt;// target position, updated by caller
    void setAimPt( vec3f aimPt );// assigns barrel ( vtx[] ) positions
    // assign vtx[].positions and return surface normal Nu
    vec3f setBarrelOrientation( vec3f ctrPos, vec3f Tw, vec3f Th );// general orientation: pt[0], pt[1] toward Tu
    void setBarrelOrientation();// calls above based on shotVel and camPos

    float dtFire = 0.2f;// 5 rounds/second
    float tElap = 0.0f;
    bool getFiringVelocity();
    // mortar mode
    bool mortarMode = false;
    void setMortarMode( bool MM );
    float firingAngle = 1.0f;// given
    bool getMotarVelocity();
    // fire a shotBurst
    std::function<bool( vec3f, vec3f, float tFuse )> pFireFragShot = nullptr;
    float tFuse = 1.0f;// = tFlight - short time before the hit
    bool fragMode = false;// fire regular shots

    // animated movement
    vec3f movePt;
    float moveSpeed = 400.0f;
    void startMove( vec3f movePos );// trigger

    // animated turret motion
    vec3f newAimPt;
    float yawRate = 0.4f, pitchRate = 0.2f;
    bool isYawing = false;// 1st motion. rotation axis = persPt::yHat
    // vary during motion
    float varAngle = 0.0f;// angle being varied
    float angle = 0.0f, rotRate = 0.0f;// during motion to either yawAngle or pitchAngle, and for rate
    vec3f Trot;// rotation axis during pitch motion

    void startAiming( vec3f aimPt );
    bool update_turret( float dt );// returns true while moving
    bool isAiming = false;
    bool fireOnceAimed = false;
    bool fireAshot();

    void init( std::istream& is, bool& rTrigger, std::function<bool( vec3f, vec3f, bool )> p_FireShot );
    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    void update_rw();// position and scale baseImg
    virtual void draw( sf::RenderTarget& RT ) const;

    persCannon(){}
    virtual ~persCannon(){}
};

#endif // PERSCANNON_H
