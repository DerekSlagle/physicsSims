#ifndef PERSPYRAMID_H
#define PERSPYRAMID_H

#include "persPt.h"


class persPyramid : public persPt
{
    public:

    vec3f pt[4];// pt[3] is the apex
    vec3f Nb;// unit vector perpendicular to base. No side normals
    sf::Vertex vtx[12];// base: 0,1,2. Sides: 0,1,3  1,2,3  2,0,3
    bool doDrawSide[4];// find in update to use in draw
    float b, h;// base length and height of the pyramid

    // function members
    void init( std::istream& is );
    vec3f getPosition()const{ return pos; }
    virtual void setPosition( vec3f Pos );
    void setVtxPositions();
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    persPyramid(){}
    virtual ~persPyramid(){}

};

#endif // PERSPYRAMID_H
