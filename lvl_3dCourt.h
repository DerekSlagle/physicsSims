#ifndef LVL_3DCOURT_H
#define LVL_3DCOURT_H

#include <Level.h>
// #include "../perspective_types/persPt.h"
#include "../perspective_types/persBlockGrid.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonValOnHit.h"

class lvl_3dCourt : public Level
{
    public:
    vec3f boxCorner[8];
    std::vector<sf::Vertex> boxSideVtxVec, boxEdgeVtxVec;// as sf::Quads, sf::Lines
    float ZboxBack = 4000.0f, ZboxFront = 0.0f;
    void updateBoundBox();

    // the ball
    persBall ball;
    float ballSpeed = 1000.0f;
    bool hitBox();

    // project ball path to plane of paddle or to wall
 //   vec3f hitPt;
    sf::Vertex ballPath[3];// from ball to next 2 collision points
    vec3f pathHitPt[2];// last 2 ballPath points
    sf::CircleShape pathDot;// where to move paddle
    bool showDot = false;
    bool showAim = false;// use aimPath to project ball path if paddle hit
    bool ballInGrid = false;// ball may be drawn by the grid it is in or by level if in none
    bool findHitPt( vec3f& Pos, vec3f& Vel )const;// writes new Vel and Pos = hitPt
    void assignPath();// when ball changes direction
    void updatePath();// frame to frame so it starts at the ball
    // project aim point. Entire box. Can't miss.
    vec3f findAimPt( vec3f P0, vec3f Vel, vec3f& Nu )const;// returns HitPt and writes surface normal
    sf::Vertex aimPath[2];// from paddle to next collision point
    sf::CircleShape aimDot;// at hit point

    // aiming
    bool fireKeyDown = false;// spacebar. Aim while held down. fire on release
    bool shiftKeyDown = false;// alternate function
    persBlockGrid* pGridLineHit = nullptr;// block hit by beam whhen aiming

    // the paddle - not a persPt type
    bool paddleHeld = false;
    float paddleR = 40.0f;
    vec3f paddlePos;
    float Kp = 0.5f;
    unsigned int numPoints = 20;
    std::vector<sf::Vertex> fanVec;// primed coordinate axes + tips = 3x sf::Lines per axis = 18 elements
    std::vector<sf::Vertex> perimVec;// sf::LinesStrip edging
    sf::Vertex ZupLine[2];
    float ZupLineLength = 100.0f;
    bool initPaddle( std::istream& is );
    void constructAxes( vec3f Zu_p );// from givenZup
    void graphPaddle();
    void updatePaddle();
    void drawPaddle( sf::RenderTarget& RT ) const;
    bool hitPaddle( vec3f posLast );
    // 2 coordinate frames. world and primed
    vec3f Xup = persPt::xu, Yup = persPt::yu, Zup = persPt::camDir;// initially coincident
    sf::Color paddleColor, paddleColorHit;
    int hitTimeLimit = 16, hitTime = 0;// change to hit color for timeLimit frames

    // targets
 //   persBlockGrid gridA;
    persBlockGrid* pGridA = nullptr;// special instance to control
    std::vector<persBlockGrid> gridVec;
    bool doKnockOut = false;// enable hits to knock out grid cells
    float gridPitch = 0.0f, gridYaw = 0.0f;// to assign gridA.Zu
    bool init_grids( std::istream& is );

    // camera movement
    float camSpeed = 100.0f;
    int move_x = 0, move_y = 0;// arrow keys assign +-1 for x or y

    // z order of ball + grids
    std::vector<persPt*> pPersSorted;
    unsigned int numToDraw = 0;// may not include ball

    // scoring
    int Score = 0;
    sf::Text scoreMsg, scoreNumMsg;

    // controls
    controlSurface multiCS;
    bool init_controls( std::istream& is );

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    lvl_3dCourt(){}
    virtual ~lvl_3dCourt(){}
};

#endif // LVL_3DCOURT_H
