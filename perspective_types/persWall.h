#ifndef PERSWALL_H
#define PERSWALL_H

#include "persLine.h"


class persWall : public persPt
{
    public:
    sf::Vertex wallQuad[4];
    persLine lineWQ[2];
    sf::Vertex sides[4];
    float W = 100.0f, H = 100.0f;
    vec3f Nu;
    vec3f pt[4];
 //   bool doDrawWQ = false;
    void setSidePositions();

    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( vec3f Pos );
    void setNu( vec3f nu );

    void init( vec3f Pos, float Width, float Height, vec3f nu, sf::Color color, const sf::Texture* p_Txt = nullptr );
    void init( vec3f posStUp, vec3f posEndUp, vec3f posStDn, vec3f posEndDn, vec3f nu, sf::Color color );
    persWall(){}
    virtual ~persWall(){}
};

#endif // PERSWALL_H
