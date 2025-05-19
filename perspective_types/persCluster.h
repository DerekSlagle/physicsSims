#ifndef PERSCLUSTER_H
#define PERSCLUSTER_H

#include<algorithm>// for std::sort() in z_order()
#include "persPt.h"

// a cluster of persPt within a bounding Rbound
// to be updated, z-ordered and drawn together
class persCluster : public persPt
{
    public:
    std::vector<persPt*> pPersVec;// all persPt objects within Rbound
    std::vector<persPt*> pTargetVec;// a subset of above for hit testing only
    std::vector<persPt*> pSortedVec;// doDraw =  true only in z_order for draw()
    std::vector<vec3f> relPosVec;
    bool doZ_order = true;
    unsigned int numToDraw = 0;// varies with view on scene
    // markers
    sf::CircleShape posCS, boundCS;
    bool showCenter = false, showBound = false;

    virtual void setPosition( vec3f Pos );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
//    virtual void update_doDraw(); base version should be good
    void z_order();
    bool hitAll( persBall& PB, float dt )const;// true if any in pTargetVec hit
    bool inBounds( vec3f Pos )const;

    // supply pointers to fully initialized objects
    bool init( const std::vector<persPt*>& p_PersVec, const std::vector<persPt*>& p_TargetVec );
    persCluster( const std::vector<persPt*>& p_PersVec, const std::vector<persPt*>& p_TargetVec )
    { init( p_PersVec, p_TargetVec ); }

    bool init();// presumes pre-filled pPersVec
    // no ctor possible for above init()

    persCluster(){}
    virtual ~persCluster(){}
};

#endif // PERSCLUSTER_H
