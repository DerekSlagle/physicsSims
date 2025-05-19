#include "persArcadeTargets.h"

void persArcadeTargets::init( std::istream& is, std::vector<spriteSheet>& SSvec )
{
    is >> pos.x >> pos.y >> pos.z;
    is >> Length >> Height >> speed;
    Rbound = 0.5f*Length;
    float angPol; is >> angPol;// orientation in x,z plane
    Nf = sinf( angPol )*persPt::xHat + cosf( angPol )*persPt::zHat;// normal to front
    vec3f tw = Nf.cross( persPt::yHat );// horizontal

    // for frontPQ
    is >> frontPQ.w >> frontPQ.h;
    frontPQ.pos = pos + 0.5f*frontPQ.h*persPt::yHat;
    frontPQ.setOrientation( Nf, persPt::yHat );
    size_t SSnum, setNum, frIdx; is >> SSnum >> setNum >> frIdx;
    char chA, chB; is >> chA >> chB;
    frontPQ.pTxt = &SSvec[SSnum].txt;
    frontPQ.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
    for( unsigned int j = 0; j < 4; ++j ) frontPQ.vtx[j].color = sf::Color::White;
    // for leftPQ
    is >> leftPQ.w >> leftPQ.h;
    leftPQ.pos = pos + 0.5f*leftPQ.h*persPt::yHat - 0.5f*Length*tw;
    leftPQ.setOrientation( Nf, persPt::yHat );
    is >> SSnum >> setNum >> frIdx;
    is >> chA >> chB;
    leftPQ.pTxt = &SSvec[SSnum].txt;
    leftPQ.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
    for( unsigned int j = 0; j < 4; ++j ) leftPQ.vtx[j].color = sf::Color::White;
    // for rightPQ
    is >> rightPQ.w >> rightPQ.h;
    rightPQ.pos = pos + 0.5f*rightPQ.h*persPt::yHat + 0.5f*Length*tw;
    rightPQ.setOrientation( Nf, persPt::yHat );
    is >> SSnum >> setNum >> frIdx;
    is >> chA >> chB;
    rightPQ.pTxt = &SSvec[SSnum].txt;
    rightPQ.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
    for( unsigned int j = 0; j < 4; ++j ) rightPQ.vtx[j].color = sf::Color::White;

    // targets
    persQuadSpin PQStemp;
    is >> PQStemp.w >> PQStemp.h;
    PQStemp.pos = pos + ( Height + 0.5f*PQStemp.h )*persPt::yHat - 0.5f*Length*tw;
    PQStemp.N0 = Nf;
 //   PQStemp.angMin = 0.0f; PQStemp.angMax = 3.1416f;
    PQStemp.M = 10.0f;
    PQStemp.I = 40000.0f;
    PQStemp.Rcm = 0.5f*PQStemp.h;
    PQStemp.rotAxis = Nf.cross( persPt::yHat );
    PQStemp.setOrientation( Nf, persPt::yHat );
    is >> SSnum >> setNum >> frIdx;
    is >> chA >> chB;
    PQStemp.pTxt = &SSvec[SSnum].txt;
    PQStemp.setTxtRect( SSvec[SSnum].getFrRect( frIdx, setNum ), chA, chB );
    for( unsigned int j = 0; j < 4; ++j ) PQStemp.vtx[j].color = sf::Color::White;
    float dPos; is >> dPos >> speed;// minimum spacing
    is >> PQStemp.angMin >> PQStemp.angMax;
    is >> isRunning;
    size_t numTargets = Length/( PQStemp.w + dPos );
    dPos = Length/numTargets;// - PQStemp.w;
    PQSvec.reserve( numTargets );
    for( size_t j = 0; j < numTargets; ++j )
    {
        PQSvec.push_back( PQStemp );
        PQStemp.pos += dPos*tw;
        std::cout << "\n PQStemp.pos = " << PQStemp.pos.x << ", " << PQStemp.pos.y << ", " << PQStemp.pos.z;
        for( unsigned int j = 0; j < 4; ++j )
        {
            PQStemp.pt[j] += dPos*tw;
            PQStemp.vtx[j].position = persPt::get_xyw( PQStemp.pt[j] );

        }
    }

    std::string chkStr; is >> chkStr;
    std::cout << chkStr;
}

void persArcadeTargets::setPosition( vec3f Pos )
{

}

void persArcadeTargets::update_doDraw()
{
    leftPQ.update_doDraw();
    rightPQ.update_doDraw();
    frontPQ.update_doDraw();
    for( persQuadSpin& PQS : PQSvec ) PQS.update_doDraw();
    doDraw = leftPQ.doDraw || rightPQ.doDraw;
}

void persArcadeTargets::update( float dt )
{
    update_doDraw();
    if( !doDraw ) return;
    // TODO: move targets
    for( persQuadSpin& PQS : PQSvec ) PQS.update(dt);
    frontPQ.update(dt);
    leftPQ.update(dt);
    rightPQ.update(dt);

    vec3f tw = Nf.cross( persPt::yHat );
    if( isRunning )
    {
        for( persQuadSpin& PQS : PQSvec )
        {
            vec3f newPos = PQS.pos + speed*dt*tw;
            if( ( newPos - pos ).dot( tw ) > 0.5f*Length )
            {
                newPos = pos + ( Height + 0.5f*PQS.h )*persPt::yHat - 0.5f*Length*tw;
                PQS.reset();
            }
            PQS.setPosition( newPos );
            if( PQS.isSpinning ) PQS.update(dt);// for the spinning
        }
    }
    else
    {
        for( persQuadSpin& PQS : PQSvec ) PQS.update(dt);
    }
}

void persArcadeTargets::resetTargets()
{
    for( persQuadSpin& PQS : PQSvec ) PQS.reset();
}

bool persArcadeTargets::hit( persBall& PB, float dt, float Cr, vec3f& P, vec3f& vu )
{
    // hit test entire area first - define a rectangle?
 //   if( !persPt::hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) ) return false;

    if( frontPQ.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
        PB.vel = PB.vel.mag()*Cr*vu;
        if( Nf.dot( persPt::camDir ) < 0.0f )// camera is in front
            PB.setPosition( P + PB.Rbound*Nf );
        else
            PB.setPosition( P - PB.Rbound*Nf );
        return true;
    }
    else if( leftPQ.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
        PB.vel = PB.vel.mag()*Cr*vu;
        if( Nf.dot( persPt::camDir ) < 0.0f )// camera is in front
            PB.setPosition( P + PB.Rbound*Nf );
        else
            PB.setPosition( P - PB.Rbound*Nf );
        return true;
    }
    else if( rightPQ.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
    {
        PB.vel = PB.vel.mag()*Cr*vu;
        if( Nf.dot( persPt::camDir ) < 0.0f )// camera is in front
            PB.setPosition( P + PB.Rbound*Nf );
        else
            PB.setPosition( P - PB.Rbound*Nf );
        return true;
    }

    // the targets
    if( Nf.dot( persPt::camDir ) < 0.0f )// camera is in front
    {
        for( persQuadSpin& PQS : PQSvec )
        {
            if( PQS.hitSpin( PB, dt, Cr, P, vu ) )
            {
                PB.setPosition( P + PB.Rbound*Nf );
                PB.vel = vu*PB.vel.mag();
                return true;
            }
        }
    }
    else
    {
        for( persQuadSpin& PQS : PQSvec )
        {
            if( PQS.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
            {
                PB.vel = PB.vel.mag()*Cr*vu;
                PB.setPosition( P - PB.Rbound*Nf );
                return true;
            }
        }
    }

    return false;
}

void persArcadeTargets::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    for( const persQuadSpin& PQS : PQSvec ) PQS.draw(RT);
    frontPQ.draw(RT);
    leftPQ.draw(RT);
    rightPQ.draw(RT);
 //   for( const persQuadSpin& PQS : PQSvec ) PQS.draw(RT);
}
