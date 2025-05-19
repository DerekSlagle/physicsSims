#ifndef LVL_INSIDEPERSPECTIVE_H
#define LVL_INSIDEPERSPECTIVE_H

#include<algorithm>
#include "Level.h"
#include "../perspective_types/persLine.h"
#include "../spriteSheet.h"
#include "../perspective_types/persChair.h"
#include "../perspective_types/persCorner.h"
#include "../perspective_types/persRoom.h"
#include "../perspective_types/persWall.h"
#include "../perspective_types/persTrail.h"
#include "../perspective_types/persDoor.h"
#include "../perspective_types/persPyramid.h"
#include "../button_types/joyButton.h"
#include "../button_types/pushButton.h"
#include "../button_types/slideBar.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"

class lvl_insidePerspective : public Level
{
    public:

    persDoor doorA;
 //   persRoom roomB;
    std::vector<persRoom*> pRoomVec;
    persRoom* pCurrentRoom = nullptr;// nullptr if outside

    sf::Vertex viewCross[4];// cross at at persPt::X0, persPt::Yh
    float pipLength = 10.0f;
    float camVelXZscale = 0.1f;
    float camSpeed = 1.0f, yawRate = 1.0f, pitchRate = 1.0f, rollRate = 1.0f;
    int move_LtRt = 0, move_UpDown = 0;// -1: Lt, Down +1: Rt Up
    float move_xyVel = 300.0f;// using arrow keys to move in xu or yu directions
    bool amOutside = true;

    sf::Vertex trueHorizon[2];
    sf::Vertex upperPane[4];
    void updatePane();

    // things to draw
    std::vector<spriteSheet> SSvec;
    bool loadSpriteSheets();

    // another try at view through a door - doorA
 //   sf::RenderTexture picture;// draw objects behind door
 //   sf::Vertex pictQuad[4];// display here
 //   void updateDrawFill( float dt );
    std::vector<persBox_quad> aBoxVec;// in front of doorA
    std::vector<persBox_quad> bBoxVec;// behind doorA
    void updateDraw_outFromIn( sf::RenderTarget& RT );

    // outdoor items
    std::vector<persQuad> PQvec_out;// walls, doors, windows, wall art
    std::vector<persTrail> trailVec;// toRep below
    unsigned int numToDrawOut = 0;
    std::vector<persPt*> pPtSortVec_out;
    bool init_outdoors();
    void update_outdoors( float dt );
    void update_global( float dt );
    // misc
    persPyramid pyr1;

    // messages
    std::vector<sf::Text> msgVec, msgNumVec;

    // controls
    joyButton jbCamButt;
    controlSurface multiCS;
    bool init_controls();

    // functions
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    lvl_insidePerspective():Level() { std::cout << "\nHello from lvl_insidePerspective ctor\n"; }
    virtual ~lvl_insidePerspective(){ for( persRoom* pPR : pRoomVec ) delete pPR; }

};

#endif // LVL_INSIDEPERSPECTIVE_H
