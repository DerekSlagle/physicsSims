#ifndef PERSSHADOW_H
#define PERSSHADOW_H

#include "persPt.h"

// a shadow as a translucent underlay on the scene
class persShadow : public persPt
{
    public:
    static bool shadowsOn;// for easy toggle on/off of all shadows. *this still draws *pPQ
    sf::Vertex shade[4];// sf::Quad for the shaded area
    persQuad* pPQ = nullptr;// shadow applies to this type only
    vec3f Pgrd[4];// the calculated points where the light ray meets the ground
    vec3f sHat;// unit vector in direction of light. This could be calculated at each *pPt[] for a given source position

    void init( persQuad& rPQ, vec3f& s_Hat, sf::Color color );// s_Hat by rfc = future ability to have changing direction of light
    void setPQ( persQuad& rPQ );// after init to change which persQuad bound to
    virtual void update( float dt );
    void update( vec3f s_Hat, float dt ){ sHat = s_Hat; update(dt); }
    virtual void draw( sf::RenderTarget& RT ) const;

    persShadow(){}
    persShadow( persQuad& rPQ, vec3f& s_Hat, sf::Color color ){ init( rPQ, s_Hat, color ); }
    virtual ~persShadow(){}
};

class shadowAdapter : public persPt
{
    public:
    persShadow shPair[2];
    persBox_quad* pPBQ = nullptr;

    void init( persBox_quad& rPBQ, vec3f& s_Hat, sf::Color color );
    void setPBQ( persBox_quad& rPBQ );// change which PBQ *this is bound to
    void setPQ_pair( persBox_quad& rPBQ );// utility: called by above and below to bind shPair to PQs
    void setPQ_pair();// adjust to change in lighting. Is called in update( vec3f, float ) below
    virtual void update( float dt );
    void update( vec3f s_Hat, float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    shadowAdapter(){}
    shadowAdapter( persBox_quad& rPBQ, vec3f& s_Hat, sf::Color color ){ init( rPBQ, s_Hat, color ); }
    virtual ~shadowAdapter(){}
};

#endif // PERSSHADOW_H
