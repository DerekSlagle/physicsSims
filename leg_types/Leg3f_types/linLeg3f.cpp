#include "linLeg3f.h"

void linLeg3f::init( vec3f Pos0, vec3f PosF )// 1st leg
{
    pos0 = Pos0;
    posF = PosF;
    len = ( posF - pos0 ).mag();
    prev = next = nullptr;
    Tu = ( posF - pos0 )/( posF - pos0 ).mag();
}

void linLeg3f::init( Leg3f& rPrev, vec3f PosF )// mid legs
{
    init( rPrev.getPos( rPrev.len ), PosF );
    prev = &rPrev;
    rPrev.next = this;
}

void linLeg3f::init( Leg3f& rPrev, Leg3f& rNext )// last (tie) leg
{
    init( rPrev.getPos( rPrev.len ), rNext.getPos( 0.0f ) );
    prev = &rPrev;
    rPrev.next = this;
    next = &rNext;
    rNext.prev = this;
}

void linLeg3f::init( vec3f Pos0 , Leg3f& rNext )// entry leg
{
    init( Pos0, rNext.pos0 );
    prev = nullptr;
    next = &rNext;// no change to rNext.prev so path is closed
}
