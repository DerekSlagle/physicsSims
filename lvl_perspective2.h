#ifndef LVL_PERSPECTIVE2_H
#define LVL_PERSPECTIVE2_H

#include<algorithm>
#include "Level.h"
#include "../perspective_types/persLine.h"
#include "../spriteSheet.h"

#include "../initControlFuncs_perspective2.h"

#include "../button_types/joyButton.h"
#include "../button_types/pushButton.h"
//#include "../button_types/buttonValOnHit.h"// included by: initControlFuncs_perspective2.h
//#include "../button_types/controlSurface.h"// included by: initControlFuncs_perspective2.h
//#include "../button_types/radioButton.h"// included by: initControlFuncs_perspective2.h
#include "../button_types/buttonList.h"
#include "../vectorBasis.h"
#include "../edgeHit.h"
//#include "../perspective_types/shotBurst.h"// included by: initControlFuncs_perspective2.h
#include "../perspective_types/persDrone.h"
#include "../leg_types/Leg3f_types/LinLeg3f.h"
#include "../leg_types/Leg3f_types/cirLeg3f.h"
//#include "../perspective_types/homingShot.h"// included by: initControlFuncs_perspective2.h
//#include "../perspective_types/persCannon.h"// included by: initControlFuncs_perspective2.h
//#include "../perspective_types/persQuadSpin.h"
#include "../perspective_types/persArcadeTargets.h"
#include "../perspective_types/persMat.h"

class lvl_perspective2 : public Level
{
    public:
    sf::Vertex viewCross[4];// cross at at persPt::X0, persPt::Yh
    float pipLength = 10.0f;
    float camVelXZscale = 0.1f;
    float camSpeed = 1.0f, yawRate = 1.0f, pitchRate = 1.0f, rollRate = 1.0f;
    int move_LtRt = 0, move_UpDown = 0;// -1: Lt, Down +1: Rt Up
    float move_xyVel = 300.0f;// using arrow keys to move in xu or yu directions
 //   bool amOutside = true;

    sf::Vertex trueHorizon[2];
    sf::Vertex upperPane[4];
    void updatePane();

    // things to draw
    std::vector<spriteSheet> SSvec;
    std::vector<persBox_quad> PBQ_toCopy;
    std::vector<persBox_quad> PBQvec;
    std::vector<persQuad> nohitPQvec, targetPQvec;
    persQuad tree_toCopy, treeBase_toCopy;
    persQuad target_toCopy;

    std::vector<persBall> targetBallVec;// try out new bool persBall::isSighted() to find aimPt
    persBall* pTgtBall = nullptr;
    bool init_terrain( const std::string& inFileName );

    // persMat = plotted function
 //   persMat mat_1;
 //   persMat mat_cone, mat_parabola, mat_hyperbola;// of revolution
 //   bool init_mats( const char* fileName );
 //   void update_mats( float dt );
    // slide without friction on a mat
 //   persBall matBall;

 //   bool doAnimate = false;
 //   float mbA = 1.0f;// the 'A' in y = A*r (cone) or y = A*r*r (parabola)
 //   unsigned int matID = 1;// 1 = the cone, 2 = parabola in getEtot() and update_mats()
 //   float mb_r, mb_r1;// mb_r2;// radius and 1st, 2nd time derivatives
 //   float mb_angle, mb_angle1;
 //   float mb_Lz, mb_Etot;// constants of motion Lz = r*r*angle1 and Etot
    // cache initial values
 //   float mb_r0, mb_r10;
 //   float mb_angle0, mb_angle10;
 //   float getEtot() const;// from r, r1, angle1
 //   sf::Text mb_EtotNumMsg;
 //   unsigned int mb_numReps = 1;


    // z ordering
    std::vector<persPt*> pPtAllVec;
    std::vector<persPt*> pPtSortVec;
    unsigned int numToDraw = 0;

    // things to fire with
    std::vector<persBall> shotVec;
    std::vector<persQuadAni> quadAniVec;
    bool getAniQuad( persBall& rPB, vec3f Pos );
    bool getAniQuad( vec3f Pos );// just get the quad
    bool anyShots_inUse = false, anyAnis_inUse = false;
    bool firing = false;
    bool firingCannon = false;// cannon has * to this = Fire Away!
    float fireVel = 2000.0f;
    float tFireDelay = 0.1f, tFire = 0.0f;
    float fireOfst = 80.0f;
    bool fireBall( vec3f Pos, vec3f Vel, bool addMuzzleFlash );
    vec3f getShotPos0()const { return persPt::camPos - fireOfst*persPt::yu; }
    vec3f getShotVel0()const { return ( fireVel )*persPt::camDir + 0.05f*fireVel*persPt::yu; }
    vec3f gravity;
    void hitAll( persBall& PB, float dt );
    // pixel perfect collision detection
    sf::Vertex *pVtxTCmin = nullptr, *pVtxTCx = nullptr, *pVtxTCy = nullptr;// the PQ.vtx[] with minimum texCoords and +x, +y from there
    bool hitPixel( unsigned int ssNum, sf::Color xprtColor, persQuad& PQ, vec3f hitPt )const;// true if hit over transparency Color
    std::vector<edgeHit> edgeVec;// deflect off of edges, horiz and vert only as yet

    // drone cannons
    persCannon aCannon;
    bool init_Cannons( const std::string& fileName );
    controlSurface cannonCS;
 //   bool init_CannonControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );

    // targeted shots
    bool amTargeting = false;
    bool aimingHigh = false;
    persQuad* pTargetQuad = nullptr;// which one is targeted
 //   float targetDistance = 100.0f;// as calculated by persQuad::isSighted()
    vec3f aimPt;// assigned in updateTargeting()
    bool getFiringVelocity();// false if h too low or R too great for given fireVel. writes vel
    bool getFiringVelocity( vec3f& vel, float& tFlight );// false if h too low or R too great for given fireVel. writes vel
    void updateTargeting();
//    bool getHighAngle = false;
 //   bool firingDouble = false;// fire hi and low shots to hit target at once
    vec3f hiFireVel, loFireVel;
    float tDblFire = 0.0f;// count down to 0.0f
    float dtDoubleFire = 0.0f;
    sf::Text targetingMsg;// "High" or "Low"
    sf::Text angleMsg;// "High" or "Low"
    std::vector<sf::Text> targetMsgVec;
    sf::RectangleShape targetBkRect;// message background
    sf::Text *pRangeMsg = nullptr, *pElevationMsg = nullptr, *pDistanceMsg = nullptr;
    sf::Text *pflightTimeMsg = nullptr, *pAngleMsg = nullptr;
    void initTargetingMessages( std::ifstream& is );

    // a shotBurst
    shotBurst fragShotA;
    float tFuse = 0.5f;// for aimed shots. tFuse = time to explode before reaching target
    controlSurface fragCS;
//    bool init_fragControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );
    // a homingShot
    homingShot Homer;
    controlSurface homerCS;
 //   bool init_homerControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );
    // interceptor shot
    persBall iShot;// no gravity
    float tiFlight = 0.0f;
    float tiFlightMax = 3.0f;
    persPt *p_iTarget = nullptr;// has pos and vel to go by
    float get_iShot();// returns tFlight to hit

    // Hellfire shot along joined parabolic path
    vec3f hf_Pos0;// at xf from target
    vec3f hf_PosTarget;
    vec3f hf_PosShot;// this is integrated. Use to update actual shot position.
    vec3f hf_Hu;// unit vector in x,z plane = line of path
    float hf_xf, hf_xc, hf_yMax, hf_yf;// qtys in formulas
    float hBoom = 200.0f;// distance over target: hf_yf = hf_PosTarget.y + hBoom;
    float launchHellfire();// returns tFlight to hit
    void updateHellfire( float ft );// no action by gravity. Path is calculated.

    persQuadSpin PQS_1;
    persArcadeTargets PAT_1;
    vec3f PQSgravity;
    bool PQSgravityOn = true;
    sf::Text PQSangleMsg, PQSangleNumMsg, PQSangVelMsg, PQSangVelNumMsg;// update and draw in
    sf::CircleShape spinLight;// CS.pUpdate(), pDraw() lambdas
    controlSurface spinTargetCS;
    bool init_spinTargetControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );

     // messages
    std::vector<sf::Text> msgVec, msgNumVec;
    sf::Text numShotsMsg;
    sf::Text helpMsg;
    bool showHelpMsg = false;
    void update_global( float dt );
    // zoom the view
    bool isZoomed = false;
    vec3f camPosSave;
    float zoomDistance = 3000.0f;

    // programmatic motion
    vectorBasis cartBasis;// maybe xu,zu,yu
    double Rbm = 100.0, Rbm_1 = 0.0;
    double thAngle = 0.0, thAngle_1 = 0.0;
    double phiAngle = 0.0, phiAngle_1 = 0.0;
    float tElapBM = 0.0f;
    persBall BMball;// isMoving = false. Motion is assigned not pos += vel*dt
    bool BM_isMoving = false;
    sf::Vertex BMrod[2];
    persQuad BMbase;
    bool init_BasisMotion( const std::string& fileName );
    void updateBasisMotion( float dt );
    controlSurface basisCS;
 //   bool init_basisControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );
 //   bool init_basisControl( const std::string& fName );
    // leg3f path for drones
    Leg3f* pDronePath = nullptr;// dynamic. delete in cleanup
    bool init_aDrone( const std::string& fileName );
    bool initDronePath( Leg3f*& pLeg0, const std::string& inFileName );
    persDrone aDrone;
    Leg3f_adapter aDroneAdapter;
    controlSurface droneCS;
    bool init_droneControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );// KEEP. Too many objects to pass

    // controls
    joyButton jbCamButt;// navigation
    buttonList controlList;// each button = controlSurface.homeButt
    controlSurface multiCS;
    multiSelector* init_controlMS( controlSurface& rCS, buttonValOnHit*& pStrip, sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );
    bool init_MultiControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );
 //   bool init_MultiControl( const std::string& fName );
    bool init_controls( const std::string& fileName );
 //   buttonRect mortarButt;
 //   buttonRect fragButt;

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    lvl_perspective2():Level() { std::cout << "Hello from lvl_perspective2 ctor\n"; }
    void cleanup(){ Leg3f::destroy3fPath( pDronePath ); }
    virtual ~lvl_perspective2() { cleanup(); }
};

#endif // LVL_PERSPECTIVE2_H
