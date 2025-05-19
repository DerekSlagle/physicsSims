#ifndef LVL_JUSTDRAW_H
#define LVL_JUSTDRAW_H

#include "Level.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/slideBar.h"
#include "../button_types/joyButton.h"
#include "vec2f.h"
#include "../spriteSheet.h"

class lvl_JustDraw : public Level
{
    public:
    std::vector<sf::Vertex> vtxVec;// for Bezier play
    std::vector<sf::Vertex> vtxVec2;// for triangle strip experiment
    std::vector<sf::Vertex> swatchVec;// quads firm color pallete
    sf::Color selColor;
    sf::Text selColorMsg;

    // for the triangle strip draw
    const sf::Texture* pTriStripTxt = nullptr;

    // drawing to sf::RenderTexture
    sf::RenderTexture* pRendTxt = nullptr;// dynamically allocate
    sf::Image rendImg;// copy from Texture
    sf::Vertex pictQuad[4];// render unto
    const sf::Texture* pPictTxt = nullptr;// from above
    bool initRendTxt( std::istream& is );
    void updateRendTxt();
    controlSurface viewCS;
    bool init_viewCS( std::istream& is );
    sf::Vector2f viewPos_curr, viewSz_curr;// adjust by controls
    sf::Vector2f txtImgSz;// as created
    bool hitPict()const;// true if mouse over pict
    bool hitPixel()const;// true if mouse over pictClearColor
    sf::CircleShape hitLight;
    // drawn upon rendTxt
    sf::Color pictClearColor;
    sf::RectangleShape rectA;
    sf::Text inMsg;

    // from spriteSheet
    std::vector<spriteSheet> SSvec;

    // Bezier
    unsigned int numPoints = 100;
    sf::Vector2f bezOrigin;
    float bezScaleX = 100.0f, bezScaleY = 100.0f;
    float bezU = 0.0f, bezH = 0.0f;// bezU: 0 to 1, bezH: 0 to 1 where bezH = P( bezU ) a polynomial
    float bezK1 = 0.0f, bezK2 = 0.0f, bezK3 = 0.0f, bezK4 = 0.0f;// polynomial coefficients
    void init_Bezier( std::istream& is );
    void plotBezCurve();
    void plotBezAxes();
    std::vector<sf::Vertex> bezVtxVec2;// for curve slope
    sf::Vertex bezAxes[8];// and shaded lines u = 1 and y = 1

    bool init_triStrip( std::istream& is );

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    lvl_JustDraw():Level() { std::cout << "Hello from lvl_JustDraw ctor\n"; }
    void cleanup(){ if( pRendTxt ) delete pRendTxt; }
//    void cleanup(){}
    virtual ~lvl_JustDraw() { cleanup(); }

};

#endif // LVL_JUSTDRAW_H
