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

    // attempt #2 at a door fill image
    bool doDrawFill = true;// always draw pictQuad. This toggles calls to any updateDraw() on/off
    void set_doDrawFill( bool ddf );
    sf::Vertex pictQuad[4];
    sf::RenderTexture* pRendTxt = nullptr;
    std::function<void( sf::RenderTarget& )> updateDraw_inFront = nullptr;
    std::function<void( sf::RenderTarget& )> updateDraw_behind = nullptr;
    sf::Color clearColor_inFront, clearColor_behind;
 //   void initFill( sf::RenderTexture& rRendTxt, std::function<void( float, sf::RenderTarget& )> updateDrawInFront, std::function<void( float, sf::RenderTarget& )> updateDrawBehind );
    void initFill( std::function<void( sf::RenderTarget& )> updateDrawInFront, std::function<void( sf::RenderTarget& )> updateDrawBehind );
    void updateDrawFill();
    static float fracOpenMin;// for fill image to be updated and drawn

    void init( spriteSheet& rSS, unsigned int SetNum, unsigned int FrIdx, char MoveDirection, vec3f Pos, vec3f nu, float Scale, sf::Color frameColor );
    void init( std::istream& is, spriteSheet& rSS );
    void setPosition( vec3f  Pos ){}
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    void setOpenValue( float FractionOpen );
    void startMotion( float FractionOpen );

    persDoor(){}
    persDoor( spriteSheet& rSS, unsigned int SetNum, unsigned int FrIdx, char MoveDirection, vec3f Pos, vec3f nu, float Scale, sf::Color frameColor )
    { init( rSS, SetNum, FrIdx, MoveDirection, Pos, nu, Scale, frameColor ); }
    persDoor( std::istream& is, spriteSheet& rSS )
    { init( is, rSS ); }
    virtual ~persDoor(){ if( pRendTxt ) delete pRendTxt; }
};

#endif // PERSDOOR_H

