#ifndef PERSFANWIDE_H
#define PERSFANWIDE_H

#include "persFan.h"


class persFanWide : public persFan
{
    public:

    std::vector<sf::Vertex> sideVtxVec;// 2 x ( numPoints+1 )
    spriteSheet* pSSside = nullptr;
    unsigned int sideSetNum = 0;
    sf::Color sideColor;
    float Width = 40.0f;
    bool doDrawSide = true;// false if no part is facing camera
    void assignSideTexCoords( unsigned int SetNum, unsigned int FrIdx, unsigned int txtSteps = 1 );

    unsigned int idxLo = 0, idxHi = 0;// index to 1st and last segment facing camera

    virtual void updateCircle();// update Vertex positions
    virtual void updateShadow( vec3f lightDir, vec3f Nsurf, float yGround );
 //   void find_idxLoHi();// range of side elements facing the camera
 //   void updateCircle_LoHi();// update only on side in view

    virtual void setPosition( vec3f Pos );
 //   virtual void update( float dt );// base version is good
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void initPerim( sf::Color PerimColor );// fills perimVec. call after init()

    // just a circle or all numPoints given
    void init( std::istream& is, spriteSheet* p_SS, spriteSheet* p_SSside );
    persFanWide( std::istream& is, spriteSheet* p_SS, spriteSheet* p_SSside ){ init( is, p_SS, p_SSside ); }

    persFanWide(){}
    virtual ~persFanWide(){}

    protected:
    private:
};

#endif // PERSFANWIDE_H
