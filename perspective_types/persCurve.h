#ifndef PERSCURVE_H
#define PERSCURVE_H

#include "persPt.h"

class persCurve : public persPt
{
public:
    std::vector<vec3f> ptVec;
    std::vector<sf::Vertex> vtxVec;
    vec3f Xup, Yup, Zup;// curve in X,Y plane though may osculate in Z
    void constructAxes( vec3f Zu_p );// from given Zup and existing Yup
    sf::CircleShape posCS;// at pos

    // args: param, position
    std::function<void(float,vec3f&)> pInitFunc = nullptr;
    float paramMin = 0.0f, paramMax = 1.0f;
    char rotSense = 'L';// ccw = left hand. 'R' = cw = right hand

    // draw the visible portions
    unsigned int idxBeginA = 0, NvtxA = 0;// draw if Nvtx > 1
    unsigned int idxBeginB = 0, NvtxB = 0;// draw if Nvtx > 1


    virtual void update_doDraw();
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( vec3f Pos );

    persCurve(){}
    virtual ~persCurve(){}

    void init( std::istream& is, std::function<void(float,vec3f&)> p_InitFunc );
    persCurve( std::istream& is, std::function<void(float,vec3f&)> p_InitFunc ){ init( is, p_InitFunc ); }
    // add to end of given curve. Will have opposing sense
    void init( const persCurve& prevCurve, sf::Color color, float parMin, float parMax, unsigned int N, std::function<void(float,vec3f&)> p_InitFunc );
    persCurve(  const persCurve& prevCurve, sf::Color color, float parMin, float parMax, unsigned int N, std::function<void(float,vec3f&)> p_InitFunc )
    { init( prevCurve, color, parMin, parMax, N, p_InitFunc ); }
};

#endif // PERSCURVE_H
