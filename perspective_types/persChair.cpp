#include "persChair.h"

bool persChair::init( std::istream& is, spriteSheet& rSS )
{
    if( !is ) return false;

    is >> pos.x >> pos.y >> pos.z >> sz.x >> sz.y >> sz.z;
    is >> backHeight;
    vec3f Nft; is >> Nft.x >> Nft.y >> Nft.z;
    size_t setNum, frIdx; is >> setNum;
    pSS = &rSS;
    Rbound = sz.mag()/2.0f;

    // form basis
    Nft /= Nft.mag();// check
    vec3f tw = Nft.cross( persPt::yHat ); tw /= tw.mag();
    vec3f th = tw.cross( Nft );
    vec3f posQ;

    // front Q
    posQ = pos + (sz.z/2.0f)*Nft;
    ftQ.init( posQ, sz.x, sz.y, Nft, sf::Color::White, &( pSS->txt ) );
    char chA, chB;// relates to rotation or flipping of image
    is >> frIdx >> chA >> chB;
    ftQ.setTxtRect( pSS->getFrRect( frIdx, setNum ), chA, chB );
    // back Q
    posQ = pos - (sz.z/2.0f)*Nft + th*( backHeight - sz.y )/2.0f;
    bkQ.init( posQ, sz.x, backHeight, -Nft, sf::Color::White, &( pSS->txt ) );
    is >> frIdx >> chA >> chB;
    bkQ.setTxtRect( pSS->getFrRect( frIdx, setNum ), chA, chB );
    // top Q
    posQ = pos + (sz.y/2.0f)*th;
    topQ.init( posQ, sz.x, sz.z, th, sf::Color::White, &( pSS->txt ) );
    is >> frIdx >> chA >> chB;
    topQ.setTxtRect( pSS->getFrRect( frIdx, setNum ), chA, chB );

    // left Q
    posQ = pos - (sz.x/2.0f)*tw + th*( backHeight - sz.y )/2.0f;
    ltQ.init( posQ, sz.z, backHeight, tw, sf::Color::White, &( pSS->txt ) );
    is >> frIdx >> chA >> chB;
    ltQ.setTxtRect( pSS->getFrRect( frIdx, setNum ), chA, chB );
    // right Q
    posQ = pos + (sz.x/2.0f)*tw + th*( backHeight - sz.y )/2.0f;
    rtQ.init( posQ, sz.z, backHeight, tw, sf::Color::White, &( pSS->txt ) );
    is >> frIdx >> chA >> chB;
    rtQ.setTxtRect( pSS->getFrRect( frIdx, setNum ), chA, chB );

    return true;
}

void persChair::update( float dt )
{
    update_doDraw();

    if( doDraw || isMoving )
    {
        ftQ.update(dt);
        bkQ.update(dt);
        topQ.update(dt);
        ltQ.update(dt);
        rtQ.update(dt);
    }

    return;
}

/*
// this works worse than the version below
void persChair::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    vec3f sep = persPt::camPos - pos;

    if( sep.dot( ftQ.Nu ) > 0.0f )  // viewer is in front
    {
        bkQ.draw(RT);
        ltQ.draw(RT);// lt or rt 1st depends if viewing from left or right?
        topQ.draw(RT);
        rtQ.draw(RT);
        ftQ.draw(RT);
    }
    else// viewer is behind
    {
        ftQ.draw(RT);
        rtQ.draw(RT);// lt or rt 1st depends if viewing from left or right?
        topQ.draw(RT);
        ltQ.draw(RT);
        bkQ.draw(RT);
    }
}
*/

// This seems to be correct! Draw top between lt and rt
void persChair::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    vec3f sep = persPt::camPos - pos;

    if( sep.dot( ftQ.Nu ) > 0.0f )  bkQ.draw(RT);// am in front
    else ftQ.draw(RT);// am behind

    if( sep.dot( rtQ.Nu ) > 0.0f )// am right of
    {
        ltQ.draw(RT);
        topQ.draw(RT);
        rtQ.draw(RT);
    }
    else// am left of
    {
        rtQ.draw(RT);
        topQ.draw(RT);
        ltQ.draw(RT);
    }

    if( sep.dot( ftQ.Nu ) > 0.0f )  ftQ.draw(RT);// am in front
    else bkQ.draw(RT);// am behind
}


void persChair::setPosition( vec3f Pos )
{
    vec3f dPos = Pos - pos;
    pos = Pos;
    ftQ.setPosition( ftQ.pos + dPos );
    bkQ.setPosition( bkQ.pos + dPos );
    topQ.setPosition( topQ.pos + dPos );
    ltQ.setPosition( ltQ.pos + dPos );
    rtQ.setPosition( rtQ.pos + dPos );
}

void persChair::setFrontNu( vec3f nu )
{

}
