#ifndef PERSFAN_H
#define PERSFAN_H

#include "persPt.h"


class persFan : public persPt
{
    public:
    // new: to support shadows. Store them here
    static std::vector<sf::VertexArray> shadowVtxArray;// storage for each instance

//    std::vector<vec3f> ptVec;
    std::vector<sf::Vertex> vtxVec;
    std::vector<float> xfVec, yfVec;// so that P[n] = xfVec[n]*Xup + yfVec[n]*Yup
    bool isCircle = true;// false if radius varies
    // local "primed" coordinates
    vec3f Xup, Yup, Zup;
  //  float fanR = 40.0f; use Rbound
    unsigned int numPoints = 20;
    spriteSheet* pSS = nullptr;
    void assignTexCoords( bool facingZ );
    float Zup_sep_last = 1.0f;// assignTexCoords when persPt::camDir.dot( Zup ) changes sign
    void constructAxes( vec3f Zu_p );// from given Zup and existing Yup
    void graphCircle();// calculate xf, yf each frame and update Vertex positions
    void initCircle();// assign xfVec, yfVec at init. Circle shape only
    void initCircle( std::function<float(float)> R );// assign xfVec, yfVec at init. Given polar function R(angle)
    virtual void updateCircle();// update Vertex positions - other version in persFanWide
    void updateShading( vec3f lightDir );// vertex.color assigned
    sf::Color fanColor, perimColor, shadowColor;// reference

    // a statically allocated perimeter
    std::vector<sf::Vertex> perimVec;// may leave empty
    virtual void initPerim( sf::Color PerimColor );// fills perimVec. call after init()
    void addShadow();// push_back a VertexArray into the static vector above
    virtual void updateShadow( vec3f lightDir, vec3f Nsurf, float yGround );// persFanWide
    sf::VertexArray* pShadowVA = nullptr;// do not assign until all shadows pushed back
    bool doDrawShadow = false;// assign in updateShadow() may not =  doDraw

    // change orientation of basis
    void pitch( float dAngle );
    void yaw( float dAngle );
    void roll( float dAngle );

    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    void update( vec3f rotAxis, float rotSpeed, float dt );// spinning
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual bool hit( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const;// stationary only
 //   virtual void update_doDraw(); base version ok

    // fan is a circle with Rbound = radius
    bool init( const char* fName, spriteSheet* p_SS = nullptr );// open file then call below
    void init( std::istream& is, spriteSheet* p_SS = nullptr );
    persFan( std::istream& is, spriteSheet* p_SS = nullptr ){ init( is, p_SS ); }

    // fan is a polar function with Rbound = maximum radius
    bool init( const char* fName, std::function<float(float)> R, spriteSheet* p_SS = nullptr );// open file then call below
    void init( std::istream& is, std::function<float(float)> R, spriteSheet* p_SS = nullptr );
    persFan( std::istream& is, std::function<float(float)> R, spriteSheet* p_SS = nullptr ){ init( is, R, p_SS ); }

 //   persFan& operator = ( const persFan& PF );
 //   persFan( const persFan& PF ){ *this = PF; }

    persFan(){}
    virtual ~persFan();
};

#endif // PERSFAN_H
