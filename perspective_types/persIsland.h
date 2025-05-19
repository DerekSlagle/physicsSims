#ifndef PERSISLAND_H
#define PERSISLAND_H

//#include "persLine.h"
#include "persPt.h"

class persIsland : public persPt
{
    public:
    static const std::vector<persBox_quad>* pBoxTemplateVec;
    std::vector<persBox_wf> box_wfVec;
    std::vector<persBox_quad> quadBoxVec;// instances in use
    std::vector<persQuad> quadVec;
    bool is_empty()const { return box_wfVec.empty() && quadBoxVec.empty() && quadVec.empty();  }

    std::vector<persBox_quad*> pPBQ_targetVec;// targets
    static std::vector<persBall>* pShotVec;
    static std::vector<persQuadAni>* pAniVec;
    static const bool* pAnyShots_inUse;
    static std::function<bool( persBall&, vec3f )> pGetAniQuad;

    std::vector<persPt*> p_persPtVec;// sorted by distance (descending) for drawing
    bool isInsideR1 = false;// act on changes in value

    sf::RenderTexture picture;
    persQuad pictQuad;// located at Pos0 - Rb*persPt::camDir
//    vec3f pos;// center of island
//    float Rbound = 1.0e3f;// distance to take the picture from
    float Rcam = 1000.0f, Rview = 1200.0f;// Rview > Rcam to magnify image
    float R1 = 5000.0f;// full draw when separation distance < R1. present picture when > R1
    float distSq = 1.0f;// updated distance to camera, squared
    float viewAlignMin = 0.998f;// update picture when outside R1 if camDir.dot( sepU ) < viewAlignMin
 //   bool doDraw = true;

    // diagnostic
    sf::CircleShape alignInd;// indicates a pictQuad alignment
    float tLimitInd = 0.2f, tElapInd = 0.0f;
    sf::CircleShape doDrawInd;// green if doDraw, red if not

    bool init( const std::string& fName, std::vector<spriteSheet>& SSvec );
    virtual void update( float dt );
    virtual void update_doDraw();
    virtual void draw( sf::RenderTarget& RT ) const;

    void updateIsland( float dt );// good from any camPos
    void drawIsland( sf::RenderTarget& RT ) const;// A NOT const version ?
    void takePicture( vec3f sepU, float dt = 0.0f );// sepU points from Pos0 to camera

    persIsland(){}
    persIsland( const std::string& fName, std::vector<spriteSheet>& SSvec ){ init( fName, SSvec ); }
    ~persIsland(){}

};

#endif // PERSISLAND_H
