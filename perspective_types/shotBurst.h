#ifndef SHOTBURST_H
#define SHOTBURST_H

#include "persPt.h"


class shotBurst : public persBall
{
    public:
   //     bool lvl_perpective::fireBall( vec3f Pos, vec3f Vel, bool addMuzzleFlash )
    static std::function<bool( vec3f, vec3f, bool )> pFireShot;
//    bool lvl_perpective::getAniQuad(  persBall&, vec3f )
    static std::function<bool( shotBurst& rSB )> pGetFireBall;

 //   static std::vector<persBall> fragPool;
//    static bool anyFragsInUse;
    static const unsigned int numPatterns = 3;
    static std::vector<vec3f> velVecArr[numPatterns];// unit fragment velocities
    static std::vector<vec3f> velVec;// unit fragment velocities
    static void init_stat();

    unsigned int velPattern = 0;// 0 = spherical, 1 = conical, 2 = flat
    float tBoom = 100.0f, tElap = 0.0f;// for timed fuse
    float Vfire = 3000.0f, Vfrags = 3000.0f;
    // fuse modes
    char fuseMode = 'T';// T = timed, E = elevation, P = proximity
    float Rdet = 1000.0f;// elevation or distance
    vec3f* pTargetPos = nullptr;

    void fire( vec3f Pos, vec3f Vel );// call to fire *this shot
    void getBurst();// assigns a standard velocity pattern

    virtual void init( std::istream& is );
    virtual void update( float dt );
    virtual void update( vec3f grav, float dt );
    void updateFuse( float dt );
    shotBurst(){}
    virtual ~shotBurst(){}

};

#endif // SHOTBURST_H
