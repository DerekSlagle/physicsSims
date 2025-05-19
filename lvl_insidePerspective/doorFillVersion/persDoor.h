#ifndef PERSDOOR_H
#define PERSDOOR_H

#include "persPt.h"

class persDoor : public persPt
{
    public:
    sf::Texture *pDoorTxt = nullptr;
    sf::FloatRect srcRect;// for texCoords

    vec3f quadPos[4];// 0,1 texCoords vary  2,3 positions vary
    sf::Vertex doorQuad[4];// 0,1 texCoords vary  2,3 positions vary
    // mated to quadPos above
    vec3f framePos[5];// 0 and 4 are repeats to close the line strip
    sf::Vertex frame[5];// constant position. Drawn as sf::LinesStrip

    float Wtxt = 100.0f, Htxt = 100.0f;// texture dimensions
    float W = 100.0f, H = 100.0f;// half dimensions of door
    vec3f Nu, wu, hu;// basis with Nu normal to closed door
    // motion management
    bool moving = false;
    char moveDirection = 'L';// 'L'=Left, 'R'=Right, 'U'=Up, 'D'=Down
    char moveMode = 'S';// 'S'=sliding, 'I'=swinging in, 'O'=swinging out

    float fractionOpen = 0.0f;// 0.0f to 1.0f. Swinging door through pi radians. 0.5f = edge on view
    float fractionOpenCurr = 0.0f;// seeks fractionOpen during motion
    float moveSpeed = 0.01f;// fraction/frame
    float scale = 1.0f;

    // image in open doorway - THIS EFFORT IS ABANDONED
    // I CANNOT MAKE IT WORK
    // temporary external implementation
//    sf::Vertex* vtxFill = nullptr;
//    const sf::Texture* pFillTxt = nullptr;
    // moving it here
 //   sf::RenderTexture picture;// draw objects behind door
 //   sf::Vertex pictQuad[4];
 //   vec3f* pCorner[4];// point to: pCorner[j] = doorA.framePos[k]. This is being phased out
 //   bool doDrawFill = false;
 //   std::function<void(float)> updateOutFromIn = nullptr;
 //   std::function<void(sf::RenderTarget&)> drawOutFromIn = nullptr;
 //   std::function<void(float)> updateInFromOut = nullptr;
  //  std::function<void(sf::RenderTarget&)> drawInFromOut = nullptr;
//    void initFill( std::function<void(float)> update_OutFromIn, std::function<void(sf::RenderTarget&)> draw_OutFromIn,
 //                 std::function<void(float)> update_InFromOut, std::function<void(sf::RenderTarget&)> draw_InFromOut );
//   void update_fill( float dt );

 //   void init( sf::Texture& rTxt, char MoveDirection, vec3f Pos, vec3f nu, float Scale, sf::Color frameColor );
    void init( spriteSheet& rSS, unsigned int SetNum, unsigned int FrIdx, char MoveDirection, vec3f Pos, vec3f nu, float Scale, sf::Color frameColor );
 //   void init( std::istream& is, sf::Texture& rTxt );
    void init( std::istream& is, spriteSheet& rSS );
    void setPosition( vec3f  Pos );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void setOpenValue( float FractionOpen );
    void startMotion( float FractionOpen );

    // utility for user
 //   void get_quadMap( vec3f** pQuadPos );
 //   void get_opening( vec3f* cPos );

    persDoor(){}
    persDoor( spriteSheet& rSS, unsigned int SetNum, unsigned int FrIdx, char MoveDirection, vec3f Pos, vec3f nu, float Scale, sf::Color frameColor )
    { init( rSS, SetNum, FrIdx, MoveDirection, Pos, nu, Scale, frameColor ); }
    persDoor( std::istream& is, spriteSheet& rSS )
    { init( is, rSS ); }
    virtual ~persDoor(){}
};

#endif // PERSDOOR_H
