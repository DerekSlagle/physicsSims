#ifndef PERSCHAIR_H
#define PERSCHAIR_H

#include "persPt.h"


class persChair : public persPt
{
   public:
    static bool compare( persChair* pChA, persChair* pChB ){ return pChB->getDistance() < pChA->getDistance(); }
    vec3f sz;
    float backHeight = 100.0f;
    persQuad ftQ, bkQ, topQ, ltQ, rtQ;
    spriteSheet* pSS = nullptr;
    bool init( std::istream& is, spriteSheet& rSS );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( vec3f Pos );
    void setFrontNu( vec3f nu );
    persChair(){}
    virtual ~persChair(){}
    persChair( std::istream& is, spriteSheet& rSS ){ init( is, rSS ); }
};

#endif // PERSCHAIR_H
