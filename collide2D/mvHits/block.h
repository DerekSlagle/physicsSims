#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include "regPolygon.h"

class block : public regPolygon
{
    public:
    float W, H;// dimensions
//    float r;// length of rays
 //   size_t nSides;// = ptVec.size()
 //   std::vector<vec2d> ptVec;// official
 //   std::vector<sf::Vertex> vtxVec;// visual

    block( std::istream& fin );
    block() {}
    virtual mvHit* clone() const { return new block(*this); }
    virtual ~block() {}
    virtual const char* myName() const { return "block"; }
    virtual void init( std::istream& fin );
    virtual void setRotation( float angle );
    virtual float getRotation()const;
 //   virtual bool inCircle( vec2d ctr, float R, vec2d& Pimp )const;
    virtual bool is_inMe( const ball& rB, vec2d& sep, vec2d& N, float& dSep )const;
    virtual bool intersect( ball& rB );
    // all other inherited functions should do
};

#endif // BLOCK_H_INCLUDED
