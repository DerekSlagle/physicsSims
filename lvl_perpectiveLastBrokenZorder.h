#ifndef LVL_PERPECTIVE_H
#define LVL_PERPECTIVE_H

#include<algorithm>
#include<queue>
#include <SFML/Audio.hpp>

#include "Level.h"
#include "../perspective_types/persLine.h"
#include "../perspective_types/persPendulum.h"
#include "../spriteSheet.h"
#include "../perspective_types/persIsland.h"
#include "../perspective_types/persCurve.h"
#include "../perspective_types/persShadow.h"
#include "../perspective_types/shotBurst.h"
#include "../perspective_types/persCluster.h"
#include "../button_types/joyButton.h"
#include "../button_types/pushButton.h"
#include "../button_types/slideBar.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"
#include "../button_types/radioButton.h"
#include "../button_types/intSelector.h"
#include "../pathUtils.h"
#include "../leg_types/legAdapter.h"
#include "../leg_types/Leg3f_types/linLeg3f.h"
#include "../leg_types/Leg3f_types/cirLeg3f.h"
#include "moveAdaptAccel.h"

class lvl_perpective : public Level
{
    public:

    // scene markings
    sf::Vertex horizonCross[4];// horizontal line and a vertical segment at persPt::X0
    sf::Vertex trueHorizon[2];
    float pipLength = 50.0f;
    void rotateCross();
    sf::Vertex upperPane[4], lowerPane[4];
    buttonRect* pPaneButt = nullptr;// to update and draw above. Is on CS2
    void updatePane();

    float camVel = 0.0f, camAccel = 0.1f;// camera speed and acceleration rate
    float yawRate = 0.1f, pitchRate = 0.1f, rollRate = 0.1f;// torep camVelX, Y, Z
    float camVelXZscale = 5.0f;
    int move_LtRt = 0, move_UpDown = 0;// -1: Lt, Down +1: Rt Up
    float move_xyVel = 300.0f;// using arrow keys to move in xu or yu directions

    // vary view direction while maintaining current velocity
    bool freeView = false;// for look around mode key: 'L'
    sf::Text lookAroundMsg;
    vec3f freeCamDir, freeCamXu, freeCamYu;// saved values to restore when Look around over
    // animated return
    vec3f delCamDir, delXu, delYu;// delXu = freeCamXu - persPt::xu, etc on return
    float LA_tFinal = 1.0f, LA_tElap = LA_tFinal;
    float LA_k4 = 5.0f, LA_rotSpeed = 2.0f;

    bool LShiftKeyDown = false, RShiftKeyDown = false;// more key options
    // barrel roll when freeView
    float roll_tFinal = 2.0f, roll_tElap = roll_tFinal;// tElap = 0 in handleEvent()
    void updateBarrelRoll( float dt );

    // value displays
    sf::Text camPosMsg, camXmsg, camYmsg, camZmsg;
    sf::Text angleMsg, angleNumMsg;
    sf::Text speedMsg, speedNumMsg;
    sf::Text pitchMsg, pitchNumMsg;
    sf::Text rollMsg, rollNumMsg;
    sf::Text numShotsMsg, numShotsNumMsg;
    sf::Text helpMsg;
    bool showHelp = false;

    vec3f gravity;

    // some boxes
    std::vector<persBox_wf> wfBoxVec;
    std::vector<persBox_quad> quadBoxVec;// instances in use
    std::vector<persBox_quad> boxTemplatesVec;// to copy from
    std::vector<persQuad> quadVec;
    std::vector<persQuadAni> quadAniVec;
    persQuadAni messagePQA;
    bool getAniQuad( persBall& rPB, vec3f Pos );// trigger animation at Pos (collision pt) and take ball out of use
    std::vector<spriteSheet> SSvec;
    bool loadSpriteSheets();
    persQuad* pBounceQuad = nullptr;// for testing persQuad.hit()
    std::vector<persBox_quad*> pPBQvec;// more targets
    bool init_boxes();
    // lines
    std::vector<persLine> lineVec;
    // balls
    std::vector<persBall> ballVec;// the shots
    bool anyShots_inUse = false, anyAnis_inUse = false;
    bool firing = false;
    float fireVel = 2000.0f;
    float tFireDelay = 0.1f, tFire = 0.0f;
    float fireOfst = 80.0f;
    bool fireBall( vec3f Pos, vec3f Vel, bool addMuzzleFlash );
    vec3f getShotPos0()const { return persPt::camPos - fireOfst*persPt::yu; }
    vec3f getShotVel0()const { return ( camVel + fireVel )*persPt::camDir + 0.05f*fireVel*persPt::yu; }

    float coeffRest = 1.0f;// used for  all balls
    // a "free" ball - with a mild drag force
    persBall superBall;
    float SBdragCoeff = 0.1f;
    vec3f SBpos;// restore position for when superBall.isMoving
    bool sbReturning = false;
    float sb_tTrip = 1.0f, sb_tElap = 0.0f, sb_vReturn = 1000.0f;

    // a damped harmonic oscillating ball
    persBall harmOsciBall;
    vec3f HOpos;
    float HOspring = 0.1f;
    float HOdamp = 0.1f;
    controlSurface HOballCS;// HOspring, HOdamp, mass, radius
    bool init_HOballControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );

    // a box of free balls
    std::vector<persBall> freeBallVec;
    persBox_wf multiBallBox;// sides transparent, all Nu inverted to face inward, process motion only when doDraw is true
    void updateFreeBalls( float dt );
    void setFreeBallPositions();// constant initial positions. all vel *= 0.0f
    float freeV0 = 50.0f;
    vec3f freeGrav;
    controlSurface freeBallCS;// V0, mass, Rbound, numBalls
    bool init_FreeBallControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );

    // targeted shots
    persQuad* pTargetQuad = nullptr;// which one is targeted
    float targetDistance = 100.0f;// as calculated by persQuad::isSighted()
    bool getFiringVelocity( vec3f& vel, bool getHigh = false );// false if h too low or R too great for given fireVel. writes vel
    bool getHiLoFiringAngles();// also writes difference in flight time
    void updateTargeting();
    bool getHighAngle = false;
    bool firingDouble = false;// fire hi and low shots to hit target at once
    vec3f hiFireVel, loFireVel;
    float tDblFire = 0.0f;// count down to 0.0f
    float dtDoubleFire = 0.0f;
    sf::Text hiLoAngleMsg;
    // timed angle display
    float tFireMsg = 0.0f;// count down to 0.0f
    float dtFireMsg = 0.0f;
    sf::Text fireAngleNumMsg;
    // zoom in on the target quad. Hold 'V' key
    bool isZoomed = false;
    float zoomFactor = 0.5f;// 0.5 to 10 = 1/zoomFactor
    // move camera position camStep in the camDir direction via key 'S'
    float camStep = 10000.0f;

    void hitAll( persBall& PB, float dt );

    // a calculated path
    persBall calcBall;
    vec3f cbPos0;
    vec3f cbVel0;// instead of storing in vel dm of calcBall. Now assigning vel
    float tFlight = 0.0f;
    float tFlightMax = 10.0f;
    float cbKaccel = 1000.0f;
    float cbSpeed0 = 2000.0f;

    // Terrain
    // line strips
//    persLineStrip persLS;// a starter instance
    // persQuad islands
    std::vector<persQuad> terrainPQvec;// one 10x10 array in use
    // persCurve
 //   persCurve curve1;

    // for hyper jump between these persQuad
    std::vector<persQuad> hyperVec_1, hyperVec_2;// portal pairs
    void handleHyperJump( float dt );

    // paths to drive upon
    std::vector<Leg*> pPathVec;// each Leg* owns a path
    std::vector<legAdapter> carLAvec;// each object on a path needs one
    std::vector<persPt*> pCarVec;// cars on the paths
    std::vector<persBox_quad> CarVec;// cars on the paths
    bool init_paths();

    // the islands
    persIsland* pSpecialIsland = nullptr;// which one the controls work on
    std::vector<persIsland*> pIslandVec;// dynamically allocated because sf::RenderTexture can't be copied

    // roller coaster?
    Leg3f* pCoasterPath = nullptr;// owns the path
    Leg3f_adapter L3f_adapter;// for camPos
    std::vector<persLine> coasterRailVec;
    std::vector<persBall> coasterBallVec;
    std::vector<persQuad> coasterQuadVec;
    persCluster coasterCluster;// encloses and manages coasterBallVec and coasterQuadVec
    bool camOnCoaster = false;// toggles with key 'P' and assigned false when leaving path end
    bool init_3fPath();// reads from text file
    float trackLookAhead = 300.0f;
    float trackYawRange = 0.7f, trackPitchRange = -0.4f;
    void updateCoasterView();
    // to smooth view scrolling on coaster
    std::queue<vec3f> camDirQ;
    vec3f camDirSum;
    size_t camDirQ_sz = 40;

    // a drone follows above the coaster
    persQuad droneQuad;
 //   float droneSpeed = 200.0f;
    float droneElevation = 1000.0f;// speed along track, distance over track
    float droneAccFactor = 1.0f;// as a
    Leg3f_adapter L3f_adapterDrone;
    moveAdaptAccel mvAdaptAcc;// timed acceleration periods
    std::queue<vec3f> droneAccelQueue;// use a running average acceleration to smooth Leg transitions
    void updateDrones( float dt );

    // for z ordering
    std::vector<persPt*> p_persPtVec;// sorted by distance (descending) for drawing
    std::vector<persPt*> pPtVec_4list;// sorted by distance (descending) for drawing
    std::vector<persPt*> common_pPtVec;// all but shots and anis
    size_t numToDraw = 0;// used in draw()
    void init_Zorder();
    void update_Zorder();

    // a pendulum
    persPendulum Pend;
    controlSurface PendCS;// V0, mass, Rbound, numBalls
    bool init_PendControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );
    pushButton* pPendResetButt = nullptr;
    std::vector<sf::Text> PendMsgVec, PendNumMsgVec;// all in 1

    // a shotBurst
    shotBurst fragShotA;
    controlSurface fragCS;
    bool init_fragControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );

    // adding shadow effects
    std::vector<persQuad> PqVec_4shadow;
    std::vector<persShadow> shadowVec;
    vec3f sHat;
    persBox_quad testPBQ;
    shadowAdapter shadow4_PQB;// bind to above
    std::vector<shadowAdapter> shAdaptVec;// bind to the PBQ in quadBoxVec
    sf::Color shadowColor;
    bool init_shadows();
    controlSurface shadowControl;
    bool init_shadowControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );

    // mainIslandCluster manages: terrainPQvec,
    persCluster mainIslandCluster;

    // sound use
    sf::SoundBuffer shotFireSoundBuff, shotBoomSoundBuff;
    float shotFireSoundVolume = 50.0f, shotBoomSoundVolume = 50.0f;
    std::vector<sf::Sound> shotSoundVec;
    sf::Sound* playShotSound( const sf::SoundBuffer& rSB, float Volume, vec3f soundPos );
    sf::Sound* playShotSound( const sf::SoundBuffer& rSB, float Volume );
    sf::SoundBuffer fragFireSoundBuff;
    float fragFireSoundVolume = 50.0f;

    sf::SoundBuffer fragBoomSoundBuff;
    float fragBoomSoundVolume = 50.0f;
    sf::SoundBuffer shotBounceSoundBuff;
    float shotBounceSoundVolume = 50.0f;
    sf::SoundBuffer ballVsWallSoundBuff;
    float ballVsWallSoundVolume = 50.0f;

    static bool useSounds;
    static float soundAttenuDistance;
    static float soundVolumeMin;
    bool init_sounds();
    controlSurface soundControl;
    bool init_soundControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );

    // controls
    joyButton jbCamButt;
    float autoRollSens = 0.7f;
    controlSurface multiCS;// fireVel, tFireDelay, g, camAccel, camVelXZscale, coeffRest + 6 more
    bool init_MultiControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );
    controlSurface islandCS;// viewAlignMin, Rcam, Rview, R1
    bool init_IslandControl( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );
    controlSurface CS2;// misc: 8 buttons
    buttonRect* pTargetButt = nullptr;
    pushButton* pTriggerButt = nullptr;
    buttonRect* pArButt = nullptr;
    buttonRect* pDblFireButt = nullptr;
    buttonRect* pHitAllButt = nullptr;
    bool init_CS2Control( sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );
    bool init_controls();
    void closeOtherControls(  controlSurface* pCS );// helper

    // functions
    void makeRectSolid( vec3f pos, vec3f sz, sf::Color color, std::vector<vec3f>& r_posVec, std::vector<sf::Vertex>& r_vtxVec );

    lvl_perpective():Level() { std::cout << "Hello from lvl_perpective ctor\n"; }

    virtual bool init();
    bool init_terrain();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup();
    virtual ~lvl_perpective() { cleanup(); }
};

#endif // LVL_PERPECTIVE_H



    // introducing persShadow. Coming soon!
    // THIS IS COMPLETELY BROKEN. TOTAL FAIL.
    // I'll persist a bit more but it's looking very bad just now
    // Totally inexplicable values off by X 1,000,000 etc.........
//    std::vector<persQuad> PqVec_4shadow;// 1st type to deploy on
//    std::vector<persShadow> shadowVec;// Here it is! Each shadow serves as a proxy for a persQuad
 //   vec3f sHat;// incident light direction
 //   bool init_shadows();// from file
