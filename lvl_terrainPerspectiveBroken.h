#ifndef LVL_TERRAINPERSPECTIVE_H
#define LVL_TERRAINPERSPECTIVE_H

#include<algorithm>
#include<queue>// for smoothing input
#include "Level.h"
#include "../perspective_types/persMat.h"
#include "../perspective_types/persFan.h"
#include "../perspective_types/persBox.h"
//#include "../perspective_types/persCurve.h"
#include "../perspective_types/persPtRecorder.h"
#include "../perspective_types/persCluster.h"
#include "../button_types/joyButton.h"
#include "../button_types/pushButton.h"
#include "../button_types/buttonValOnHit.h"// included by: initControlFuncs_perspective2.h
#include "../button_types/controlSurface.h"// included by: initControlFuncs_perspective2.h
#include "../button_types/buttonList.h"
#include "../utility/coordinateAxes.h"
#include "../utility/animatedTransition.h"

class lvl_terrainPerspective : public Level
{
    public:

    sf::Vertex viewCross[4];// cross at at persPt::X0, persPt::Yh
    float pipLength = 10.0f;
    float camVelXZscale = 0.1f;
    float camSpeed = 1.0f, yawRate = 1.0f, pitchRate = 1.0f, rollRate = 1.0f;
    float camAccel = 400.0f;
    bool doCoast = false;// accelerate camera vs. move at constant camSpeed
    int move_LtRt = 0, move_UpDown = 0;// -1: Lt, Down +1: Rt Up
    float move_xyVel = 300.0f;// using arrow keys to move in xu or yu directions
    vec3f gravity;
    // to smooth out banking of camera motion
    std::queue<vec3f> Yu_queue, Zu_queue;
    vec3f Yu_total, Zu_total;// total of last Yu_queue.size() pushes

    sf::Vertex trueHorizon[2];
    sf::Vertex upperPane[4];
 //   void updatePane();// now a static persPt method

    // to draw
    std::vector<persBox> persBox_vec;
    bool init_boxes( const char* fileName );

    std::vector<persMat> persMat_vec;
    bool init_mats( const char* fileName );
    void update_mats( float dt );

    std::vector<persBox_wf> PBwf_toCopy;
    std::vector<persBox_wf> PBwf_vec;
    bool init_terrain( const std::string& inFileName );

    std::vector<spriteSheet> SSvec;
    bool loadSpriteSheets( const std::string& inFileName );
 //   std::vector<persBall> ballVec;// see ballPool below
    std::vector<persQuadAni> quadAniVec;
    bool getAniQuad( persBall& rPB, vec3f Pos );
    bool getAniQuad( vec3f Pos );// just get the quad
    int numAnis_inUse = 0;

    // animate a persBox
    persBox* pCar = nullptr;
    float carSpeed = 800.0f, carTimeA = 4.0f;// time on straight between curves
    float carTurnR = 600.0f;
    std::vector< animatedTransition<float> > carAniVec;
    void init_carAnis();
    std::queue<vec3f> carYuQueue;
    vec3f carYuTotal;
    vec3f getYuAvg( vec3f Yu_curr );

    // to move upon above surfaces
    persBall coneBall;
 //   persMat* pConeMat = nullptr;
    mattAdapterPolar ma_cone;
    double Acone = 1.0f;// y scaling for the mat
    sf::Text coneEtotMsg, coneEtotNumMsg;
    controlSurface coneCS;
 //   bool init_coneControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );

    persBall parBall;
 //   persMat* pParMat = nullptr;
    mattAdapterPolar ma_parabola;
    double Apar = 1.0f;
    sf::Text parEtotMsg, parEtotNumMsg;
    controlSurface paraCS;
 //   bool init_paraControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );

    persBall paraChuteBall;
 //   persMat* pConeMat = nullptr;
    matAdapter2ndOrder ma_paraChute;
    double pcZ0 = 0.0, pcZ = 0.0, Vz = 20.0;
    double AparaChute = 1.0f;// y scaling for the mat
    sf::Text parChtEtotMsg, parChtEtotNumMsg;
    // by 2nd method
    persBall paraChuteBall2;
    double pcZ2 = 0.0f, Vz2 = 30.0;
 //   persMat* pConeMat = nullptr;
    matAdapter2ndOrder ma_paraChute2;
    sf::Text parCht2EtotMsg, parCht2EtotNumMsg;
    controlSurface para2CS;

    // 2d (x,z) motion treated separately
 //   persBall sineBall;// on sin(x)*sin(z)
 //   matAdapter2ndOrder_xz maxz_sine;
    vec3f grad_u, sineVs1, sineVs2;// vector basis on surface at initial position
    vec3f sineLaunchPos;
    persMat* pSinePM = nullptr;
    float sineV0 = 100.0f;// initial speed. V = sineV0*( v1*sineVs1 + v2*sineVs2 )
    float angV0 = 0.0f, dAngV0 = vec2f::PI/6.0f, dtLaunch = 1.0f;
    // so x10 = V.dot( persPt::xHat ), z10 = V.dot( persPt::zHat )
    bool init_matAdapterControls( std::istream& is );

    // a pool of persBalls and matAdapter2ndOrder_xz to rain from hill persMat
    bool runSineBalls = false;
    std::vector<persBall> ballPool;
    int numBalls_inUse = 0;
 //   std::vector<matAdapter2ndOrder_xz> ma_xzPool;
  //  matAdapter2ndOrder_xz* get_ball_ma_pair();
    persBall* getPoolBall();

 //   size_t matIdx = 1;// 1 = cone or 2 = parabola
 //   double getEtot()const;// for cone (0) and parabola (1)
    double getEtot( const persMat* p_PM, unsigned int idx = 1 )const;// for cone (0) and parabola (1)

    // shooting
    std::vector<persBallTimed> shotPool;
    persBallTimed* getShot( vec3f Pos, vec3f Vel );// returns * to 1st found !inUse
    vec3f getShotPos0()const { return persPt::camPos - fireOfst*persPt::yu; }
    vec3f getShotVel0()const;// { return ( fireVel )*persPt::camDir; }// + 0.05f*fireVel*persPt::yu; }
    int numShots_inUse = 0;
    bool firing = false;
    float fireVel = 2000.0f;
    float tFireDelay = 0.1f, tFire = 0.0f;
    float fireOfst = 80.0f;
    void hitAll( persBall& PB, float dt );

    // matRider - low speed collision test to ride surface?
    persBall* pMatSurfer = nullptr, *pMatBouncer = nullptr;
    persMat* pSurfMat = nullptr;
    vec3f surfMatPos, surfMatPos0;
    float surfVX0 = 0.0, surfVZ0 = 0.0;
    vec3f surfNu;// surface normal at surfPos

    // POI orbiting using persPt::updatePOI()
    float poiR = 1000.0f, poiAngVel = 1.0f;
    // use <- and -> keys for 2nd purpose. Existing = translation in persPt::xu direction
    bool RShiftKeyDown = false, LShiftKeyDown = false;
    bool keepLevel = true;// maintain persPt::xu in xHat, zHat plane
    // barrel roll key B
    float tElapRoll = 2.0f, tFinalRoll = 2.0f;// assign = 2*PI/rollRate in handleEvent(). countdown in update()
    float roll_u1 = 0.2f, roll_u2 = 0.8f;// dy/du = 0 for Bezier motion. Turn around points
    // for vertical loop: pitch
    float tElapPitch = 4.0f, tFinalPitch = 4.0f;// assign = 2*PI/rollRate in handleEvent(). countdown in update()
    float pitch_u1 = 0.2f, pitch_u2 = 0.8f;

    // automate animation
    animatedTransition<float> ani_roll, ani_pitch;
    // chained animations
    animatedTransition<float> ani_halfPitch, ani_halfRoll;// invert then roll back to level
    std::vector<animatedTransition<float>*> pAniTransVec;// usage in update_global()

    controlSurface BezierCS;
    bool init_BezierControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );
 //   float get_BezSlope( float u, float u1, float u2 )const;

    std::vector<persFan> fanVec;
    std::vector<float> rotSpeedVec;
    std::vector<char> operCodeVec;// 'N' = none, 'Y' = yaw, 'P' = pitch, 'R' = roll
    persCluster cluster_1;// try out on above fans
    coordinateAxes coordAxesA;// bind to a fan above then update and draw

/*    std::vector<persCurve> curveVec;
    bool init_curves( const char* fileName );
    bool init_curveChain( const char* fileName );
    void rotate_curveChain( char oper, float dAngle );// oper = 'Y', 'P' or 'R'
    float rotSpeedChain = 0.3f;
    bool Y_KeyDown = false, P_KeyDown = false, R_KeyDown = false;   */

    // lighting
    vec3f lightDir;
    float light_pitch = 0.0f, light_yaw = 0.0f;// straight down
    controlSurface lightingCS;
    bool init_lightingControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );

    // z ordering
    std::vector<persPt*> pPtStillVec;
    std::vector<persPt*> pPtMovingVec;
    std::vector<persPt*> pPtSortVec;
    unsigned int numToDraw = 0;
    void init_Zorder();
    void update_Zorder();
    const persPt* findSighted()const;// finds nearest if objects overlap in view. nullptr if none sighted

    // messages
    std::vector<sf::Text> msgVec, msgNumVec;
    sf::Text* pSpeedNumMsg = nullptr;
    sf::Text* pPitchNumMsg = nullptr;
    sf::Text helpMsg;
    bool showHelpMsg = false;

    // replay camera movement
    // as an object
    persPtRecorder recorder;

    // controls
    joyButton jbCamButt;// navigation
    controlSurface multiCS;
    buttonList controlList;// each button = controlSurface.homeButt
    multiSelector* init_controlMS( controlSurface& rCS, buttonValOnHit*& pStrip, sf::Vector2f HBpos, sf::Vector2f HBsz, sf::Vector2f sfcOfst, std::istream& is );
    bool init_MultiControl( multiSelector* pMS, buttonValOnHit* pStrip, std::istream& is );
    bool init_controls( const std::string& fileName );

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    void update_global( float dt );// camera movement
    virtual void draw( sf::RenderTarget& RT ) const;
    lvl_terrainPerspective();
    virtual ~lvl_terrainPerspective();

};

// friends
float get_BezSlope( float u, float u1, float u2 );
// writes new basis to args
void rotateBasisLH( vec3f tRot, float angle, vec3f& Xu, vec3f& Yu, vec3f& Zu );
void makeXuLevelLH( vec3f& Xu, vec3f& Yu, vec3f& Zu );

#endif // LVL_TERRAINPERSPECTIVE_H
