#ifndef PERSPENDULUM_H
#define PERSPENDULUM_H

#include "persPt.h"
#include "../vec3d.h"

class persPendulum : public persPt
{
    public:
    static vec3d Xu, Yu, Zu;// same as persPt versions
    persBall bob;
    sf::Vertex rod[2];// support for bob
    sf::Vertex Lvtx[2];// Angular momentum arrow
    vec3f Ltip;// maintained in update. Used in draw to find order
    bool showL = false;
    unsigned int numLoops = 1;// # of times to update motion per call to update() = smaller time step
    void updateRod();
    persQuad topQ;// support for rod and a button to hit to start/stop motion?
    // motion data
    double L = 100.0;// rod length
    double Lz = 0.0;// z-comp angular momentum is conserved ( between hits ). Scaled this is actually Lz /= m*L*L
    double GravY = -200.0;
    double calcLz()const { return phi_1*sin(th)*sin(th); }
    void updateLz(){ Lz = calcLz(); }

    double Etot = 0.0;// is conserved ( between hits ). Scaled this is actually Etot /= m*L*L
    vec3d Ltot;// used to update Lvtx
    vec3d Vel;
    double calcEtot()const;
    void updateEtot() { Etot = calcEtot(); }
    double th = 0.0, th_1 = 0.0, th_2 = 0.0;// azimuthal angle and 1st and 2nd time derivatives
    double phi = 0.0, phi_1 = 0.0, phi_2 = 0.0;// polar angle and 1st and 2nd time derivatives
    // update method: use Lz and Etot conservation or just ang_1 += ang_2*dT
 //   bool useConservedQtys = true;
    bool useConservedLz = true;
    bool useConservedEtot = true;

    void init( std::istream& is );
    void re_init();// following changes to above dms
    void updateMotion( double& sinTh, double& cosTh, double& sinPhi, double& cosPhi, double dT );// manages cached values
    virtual void update( float dt );
    virtual void update_doDraw();
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( vec3f Pos );

    bool hit( persBall& PB, float cR, float dt );
    // base vectors in spherical coordinates
    vec3d get_rHat()const{ return sin(th)*( cos(phi)*Xu + sin(phi)*Yu ) - cos(th)*Zu; }
    vec3d get_thHat()const{ return cos(th)*( cos(phi)*Xu + sin(phi)*Yu ) + sin(th)*Zu; }
    vec3d get_phiHat()const{ return cos(phi)*Yu - sin(phi)*Xu; }

    vec3d get_vel()const;
    vec3d get_accel()const;

    persPendulum(){}
    virtual ~persPendulum(){}
};

#endif // PERSPENDULUM_H
