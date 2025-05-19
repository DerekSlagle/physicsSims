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
    std::vector<sf::Vertex> vtxVec;
    std::vector<sf::Vertex> swatchVec;// quads firm color pallete
    sf::Color selColor;
    sf::Text selColorMsg;

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

    // circles
    float circleR = 40.0f;
    vec3f circlePos;
    unsigned int numPoints = 20;
    std::vector<sf::Vertex> axesVec;// primed coordinate axes + tips = 3x sf::Lines per axis = 18 elements
    std::vector<sf::Vertex> perimVec;// sf::LinesStrip edging
    spriteSheet* pSS_dime = nullptr;
    void assignTexCoords( bool facingZ );
    float axisLength = 100.0f;
    float tipL = 6.0f, tipW = 4.0f;
    float rotAngle = 0.0f, rotSpeed = 1.0f;
    float Kz = 0.01f;// make it a bowl += Kz*r*r*Zup
    bool initCircle( std::istream& is );
    void constructAxes();// from angPol and angAz
    void constructAxes( vec3f Zu_p );// from givenZup
    void graphCircle();// and re form primed basis
    void drawCircle( sf::RenderTarget& RT ) const;// and axes
    // 2 coordinate frames. world and primed
 //   vec3f Xu;//(1.0f,0.0f,0.0f), Yu(0.0f,1.0f,0.0f), Zu(0.0f,0.0f,1.0f);
    vec3f Xu = vec3f(1.0f,0.0f,0.0f), Yu = vec3f(0.0f,1.0f,0.0f), Zu = vec3f(0.0f,0.0f,1.0f);
    vec3f Xup = Xu, Yup = Yu, Zup = Zu;// initially coincident
    float angPol = 0.0f, angAz = 0.0f;// orientation of Zup, Xup to be in X.Z plane
    joyButton circleJoy;
    bool init_circleControls( std::istream& is );

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
