#ifndef PERSMAT_H
#define PERSMAT_H

#include "persPt.h"

// A 3d plot y(x,z) over an area szX by szZ
class persMat : public persPt
{
    public:

    unsigned int nCellsX = 10, nCellsZ = 10;// plot in y direction
    float szX = 100.0f, szZ = 100.0f;
    float xMin = 0.0f, xMax = 100.0f;
    float zMin = 0.0f, zMax = 100.0f;
    // calculated during plot. Defines bounding box y dimension ( szY )
    float yMin = 0.0f, yMax = 100.0f;
    float scaleX = szX/nCellsX, scaleZ = szZ/nCellsZ;
    float Cr = 1.0f;// coefficient of restitution

    std::function<float(float,float)> pFxz = nullptr;
    std::function<double(double,double)> pFx = nullptr, pFz = nullptr;// partial derivatives of Fxz
    std::vector<vec3f> matVec;// nCellsX*nCellsZ elements
    std::vector<sf::Vertex> vtxVec;// 2*nCellsX*nCellsZ elements for sf::Lines in x and z directions

    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT )const;
    virtual bool hit( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const;
    bool hitMat( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const;// no bound box check
    bool rideMat( persBall& PB, vec3f& matPos, vec3f& Nmat, vec3f Grav, bool onTop, float dt )const;// low speed on surface. no bound box check
    bool inBoundingBox( vec3f Pos )const;
    bool hitBoundBox( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const;
    void hitTest_test( std::istream& is, std::ostream& os )const;

    vec3f getGradient( float x, float z )const;// calls getFx_Fy() if pFx or pFz == nullptr
    // approximate partial derivs from discrete matVec positions
    bool getFx_Fy( float x, float z, float& Fx, float& Fz )const;// false if off map, etc.
    bool getFx_Fy_test( std::istream& is, std::ostream& os )const;
 //   virtual void update_doDraw();// base version is good

    // this method barely works. updated r1 is too high and must be adjusted downward after most calls
    vec3f update_polarMotion(  float Lz, float r2, float& r, float& r1, float& angPol, float dt, unsigned int numReps = 1 );// of object on surface. returns position
 // TOTAL FAIL   vec3f update_polarMotionRK(  float Lz, float& r, float& r1, float& angPol, float& angle1, float dt, std::function<float(float,float)> r2 );// returns position

    bool init( const char* fileName, std::function<float(float,float)> p_Fxz );
    void init( std::istream& is, std::function<float(float,float)> p_Fxz );
    persMat( std::istream& is, std::function<float(float,float)> p_Fxz ){ init( is, p_Fxz ); }

    persMat();
    virtual ~persMat();

    // utility
    vec3f toWorldPos( double matPosX, double matPosZ )const;
    vec3f toMatPos( vec3f worldPos )const;
};

class mattAdapterPolar
{
public:
    double r0 = 1.0f, r = 1.0f, r10 = 0.0f, r1 = 0.0f;
    double ang0 = 0.0f, ang = 0.0f, ang10 = 0.0f, ang1 = 0.0f;
    double Ly = 1.0f, Etot = 1.0f;
    unsigned int numReps = 1;
    bool doAnimate = false;
    persMat* pPM = nullptr;
    std::function<double(double,double,double)> f_r2 = nullptr;// r2 = f_r2( r, r1, ang1 );
    persPt* pRider = nullptr;// for setPosition

    void update( float dt );
    void reset();// to initial conditions
    void setPosition()const;// using dms and pSetPosition
    void init( std::istream& is, persMat& rPM, std::function<double(double,double,double)> F_r2, persPt& rRider );
    void init( double R0, double R10, double Ang0, double Ang10, persMat& rPM, std::function<double(double,double,double)> F_r2, persPt& rRider );
};

class matAdapter2ndOrder
{
public:
    double x0 = 1.0f, x = 1.0f, x10 = 0.0f, x1 = 0.0f;
    double* pZ = nullptr;// managed elsewhere. Used in calls to pPM->pFxz()
    double Etot = 1.0f;
    double Grav = 1.0;
    unsigned int numReps = 1;
    bool doAnimate = false;
    persMat* pPM = nullptr;
    bool usef_r2 = true;// vs alternate below
    std::function<double(double,double)> f_r2 = nullptr;// r2 = f_r2( x, x1 );
    persPt* pRider = nullptr;// for setPosition
    // use general x2 formula with f(x), df/dx(x) and d2f/dx2(x)
    std::function<double(double)> pf = nullptr, pf1 = nullptr, pf2 = nullptr;

    void update( float dt );
    void reset();// to initial conditions
    void reset( double Z0 );// to initial conditions
    void setPosition()const;// using dms and pSetPosition
    vec3f getVelocity( double Vz )const;
    void init( std::istream& is, persMat& rPM, std::function<double(double,double)> F_r2, persPt& rRider, double* p_Z = nullptr );
    void init( double X0, double X10, persMat& rPM, std::function<double(double,double)> F_r2, persPt& rRider, double* p_Z );
    // using pf, pf1, pf2
    void init( double X0, double X10, persMat& rPM, persPt& rRider, std::function<double(double)> p_f, std::function<double(double)> p_f1, std::function<double(double)> p_f2, double* p_Z );
};

class matAdapter2ndOrder_xz
{
public:
    double x0 = 1.0f, x = 1.0f, x10 = 0.0f, x1 = 0.0f;
    double z0 = 1.0f, z = 1.0f, z10 = 0.0f, z1 = 0.0f;
 //   double Etot = 1.0f;
    double Grav = 1.0;
    unsigned int numReps = 1;
    bool doAnimate = false;
    persMat* pPM = nullptr;
    persPt* pRider = nullptr;// for setPosition
    // use general x2 formula with f(x,z), pf/px(x,z) and p2f/px2(x,z) and for z
    std::function<double(double,double)> pFxx = nullptr;// use pPM->pFx() for 1st partial derivative
    std::function<double(double,double)> pFzz = nullptr;// use pPM->pFz() for 1st partial derivative

    void update( float dt );
    void reset();// to initial conditions
    void setPosition()const;// using dms and pSetPosition
    vec3f getVelocity()const;
    void init( std::istream& is, persMat& rPM, persPt& rRider );
    // using pf, pf1, pf2
    void init( double X0, double X10, double Z0, double Z10, persMat& rPM, persPt& rRider );// assign pFs seperately
 //   void setFxx( std::function<double(double,double)> Fxx );
 //   void setFzz( std::function<double(double,double)> Fzz );
    vec3f getGradient( float x, float z )const;
};

#endif // PERSMAT_H
