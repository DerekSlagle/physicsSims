#include "zoneGroup.h"

zoneGroup::zoneGroup()
{
    //ctor
}

zoneGroup::~zoneGroup()
{
    //dtor
}

void zoneGroup::init( std::istream& is, const std::vector<segHit*>& SegVec )
{
    if( SegVec.empty() ) return;
    segVec = SegVec;

    float xLt, yLt, xRt, yRt;// upper left and lower right
    is >> xLt >> yLt >> xRt >> yRt;
    zoneRect.left = xLt;
    zoneRect.top = yLt;
    zoneRect.width = xRt - xLt;
    zoneRect.height = yRt - yLt;
}

bool zoneGroup::hitZone( vec2d pt )
{
    if( pt.x < zoneRect.left ) return false;
    if( pt.x > zoneRect.left + zoneRect.width ) return false;
    if( pt.y < zoneRect.top ) return false;
    if( pt.y > zoneRect.top + zoneRect.height ) return false;

    return true;
}

void zoneGroup::update_mhVec( std::vector<mvHit*>& MhVec )
{
    mhVec.clear();
    for( mvHit* pMH : MhVec )
    {
        if( hitZone( pMH->pos ) )
            mhVec.push_back( pMH );
    }
}

void zoneGroup::hitAll()
{
    for( mvHit* pMH : mhVec )
        for( segHit* pSH : segVec )
            if( pSH && pMH && pSH->hit( *pMH ) )
                break;
}
