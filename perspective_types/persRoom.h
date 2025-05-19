#ifndef PERSROOM_H
#define PERSROOM_H

#include<algorithm>
//#include "persCorner.h"
#include "persTable.h"
#include "persWall.h"
#include "persDoor.h"

class persRoom : public persPt
{
    public:

    // the new stuff
    persWall wallFt, wallBk;
    persWall wallLt, wallRt;
    std::vector<persWall*> pWallVec;// to process above
    sf::Vertex roofQuad[4];// experimental
    sf::Vertex ceilingQuad[4];
    sf::Vertex floorQuad[4];
    void update_indoors( float dt );
    void update_outdoors( float dt );
    void updateDrawInFromOut( sf::RenderTarget& RT );// to support draw through open door

    vec3f Sz;// + inherited position. Translate to existing xLt, xRt, etc in init()
    bool amInsideRoom = false;// persRoom has this as a dm. Update above accordingly
    void update_amInsideRoom();
    bool restrictYposition = true;// just assign to change rule

    bool init( std::istream& is, std::vector<spriteSheet>& rSSvec );

    virtual void update_doDraw();
    virtual void update( float dt );// called only when camera control active
    void update_always( float dt );// called to support animations
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( vec3f Pos );

    // unsorted items on wall, floor and ceiling
    // visible from inside and out
    std::vector<persQuad> PQwallVec;// static doors, windows of this exterior room
    std::vector<persDoor*> pDoorVec;// active doors, windows of this room  *** OWNER *******
    // drawn only if inside the room. Unsorted on walls, etc.
    std::vector<persPt*> pPtSurfaceVec;// OWNER

    // all interior items - to be sorted
    std::vector<persPt*> pPtVec;// OWNER any persPt type - to be sorted
    // sorted for drawing
    size_t numToDraw = 0;
    std::vector<persPt*> pPtSortedVec;// sorted items in room for drawing

    // other containers
    std::vector<persQuad> PQinVec;// interior PQs - sorted
    std::vector<persChair> chairVec;// chairs - sorted

    persRoom(){}
    persRoom( std::istream& is, std::vector<spriteSheet>& rSSvec ){ init( is, rSSvec ); }
    virtual ~persRoom();
};

#endif // PERSROOM_H
