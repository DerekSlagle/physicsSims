#include "persDrone.h"

 void persDrone::init( std::istream& is, spriteSheet* pSS )
 {
    droneQuad.init(is);
    Rbound = droneQuad.Rbound;
    pos = droneQuad.pos;
    accelSum = vec3f();
    accel = vec3f();
    Tu = vec3f( 0.0f, 0.0f, 1.0f );

    size_t setNum, frIdx; is >> setNum >> frIdx;
    char Rn; is >> Rn;
    if( pSS )
    {
        droneQuad.pTxt = &( pSS->txt );
        droneQuad.setTxtRect( pSS->getFrRect( frIdx, setNum ), 'R', Rn );
    }

    is >> accelFactor;
    size_t numEle; is >> numEle;
    for( size_t j = 0; j < numEle; ++j )
        accelQueue.push( vec3f() );
 }

void persDrone::setPosition( vec3f Pos )
{
    pos = droneQuad.pos = Pos;
    if( accel.dot(accel) > 0.01f )// avoid setOrientation() calls along linLegs
    {
        droneQuad.setOrientation( accelFactor*accel, Tu );
    }
    else
        droneQuad.setOrientation( persPt::yHat, Tu );// level
}

// helper function for buffered acceleration
vec3f persDrone::getQueueAccel()
{
    accelSum += accel;
    accelQueue.push( accel );
    accelSum -= accelQueue.front();
    accelQueue.pop();
    if( accelQueue.size() > 1 )
        return accelSum*( 1.0f/accelQueue.size() );

    return vec3f();
}

void persDrone::update( float dt )
{
    droneQuad.update(dt);
    pos = droneQuad.pos;

    update_doDraw();
    if( !doDraw ) return;//{ std::cout << "\n doDraw false"; return; }


    accel = getQueueAccel();
    if( accel.dot(accel) > 0.01f )// avoid setOrientation() calls along linLegs
    {
        vec3f nu = accel/accel.mag();
        droneQuad.setOrientation( nu, Tu );
    }
    else
        droneQuad.setOrientation( persPt::yHat, Tu );// level


 //   droneQuad.update(dt);
 //   pos = droneQuad.pos;
}

void persDrone::updateGrav( vec3f grav, float dt )
{
    droneQuad.update(dt);
    pos = droneQuad.pos;

    update_doDraw();
    if( !doDraw ) return;

    accel = getQueueAccel();
    vec3f accTotal = grav - accelFactor*accel;
    accTotal /= accTotal.mag();// unit vector in direction of
    droneQuad.setOrientation( accTotal, Tu );


  //  droneQuad.update(dt);
  //  pos = droneQuad.pos;
}

void persDrone::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    droneQuad.draw(RT);
}

//void persDrone::update_doDraw()
//{
//    droneQuad.update_doDraw();
//    doDraw = droneQuad.doDraw;
//}
