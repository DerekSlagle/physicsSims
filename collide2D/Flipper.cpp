#include "Flipper.h"


void Flipper::init( std::istream& fin )
{
    flipLine.init( fin );
    unsigned int rd, gn, bu; fin >> rd >> gn >> bu;
    sf::Color clr(rd,gn,bu);
    float Rc, Re, cr; fin >> Rc >> Re >> cr;
    centerBall.init( flipLine.pos, vec2d(0.0f,0.0f), Rc, 100.0f, 1.0f, clr );
    centerBall.is_free = false;
    endBall.init( flipLine.pos + flipLine.L, vec2d(0.0f,0.0f), Re, 100.0f, 1.0f, clr );
    endBall.is_free = false;
    topSeg.init( centerBall.pos + Rc*flipLine.N, endBall.pos + Re*flipLine.N, clr );// N points up
    topSeg.testEnd1 = topSeg.testEnd2 = false;
    bottomSeg.init( endBall.pos - Re*flipLine.N, centerBall.pos - Rc*flipLine.N, clr );// N points down
    bottomSeg.testEnd1 = bottomSeg.testEnd2 = false;
}

void Flipper::resetEdges()
{
    vec2d pt1( centerBall.pos + centerBall.r*flipLine.N ), pt2( endBall.pos + endBall.r*flipLine.N );
    topSeg.pos = pt1;
    topSeg.L = pt2 - pt1;
    topSeg.vtx[0].position.x = pt1.x; topSeg.vtx[0].position.y = pt1.y;
    topSeg.vtx[1].position.x = pt2.x; topSeg.vtx[1].position.y = pt2.y;
    topSeg.len = topSeg.L.mag();
    topSeg.N.x = topSeg.L.y; topSeg.N.y = -topSeg.L.x;
    topSeg.N /= topSeg.len;

    pt1 = endBall.pos - endBall.r*flipLine.N;
    pt2 = centerBall.pos - centerBall.r*flipLine.N;
    bottomSeg.pos = pt1;
    bottomSeg.L = pt2 - pt1;
    bottomSeg.vtx[0].position.x = pt1.x; bottomSeg.vtx[0].position.y = pt1.y;
    bottomSeg.vtx[1].position.x = pt2.x; bottomSeg.vtx[1].position.y = pt2.y;
    bottomSeg.len = bottomSeg.L.mag();
    bottomSeg.N.x = bottomSeg.L.y; bottomSeg.N.y = -bottomSeg.L.x;
    bottomSeg.N /= bottomSeg.len;
}

bool Flipper::hit( ball& b )
{
    if( centerBall.hit( b ) ) return true;
    if( endBall.hit( b ) )
    {
     //   endBall.setPosition( flipLine.pos + flipLine.L );
        return true;
    }
 //   bool Hit = false;//flipLine.hit( mh );
    vec2d Rcb = b.pos - centerBall.pos;
    if( topSeg.hit( b ) )
    {
        vec2d N( topSeg.N ), Rcp( Rcb - b.r*N ), Vp( flipLine.rotVel*vec2d( -Rcp.y, Rcp.x ) );
        b.v += Vp.dot( N )*N*( 1.0f + b.Cr );
        return true;
    }

    if( bottomSeg.hit( b ) )
    {
        vec2d N( bottomSeg.N ), Rcp( Rcb - b.r*N ), Vp( flipLine.rotVel*vec2d( -Rcp.y, Rcp.x ) );
        b.v += Vp.dot( N )*N*( 1.0f + b.Cr );
        return true;
    }

    return false;
}

void Flipper::update( float dt )
{
    if( flipLine.state == 0 ) return;

    flipLine.update(dt);
    if( flipLine.state == 0 ) { endBall.v *= 0.0f; endBall.is_free = false; }
    endBall.setPosition( flipLine.pos + flipLine.L );
    vec2d RceN( -flipLine.L.y, flipLine.L.x );
    endBall.v = flipLine.rotVel*RceN;
    resetEdges();
//    topSeg.init( centerBall.pos + centerBall.r*flipLine.N, endBall.pos + endBall.r*flipLine.N, endBall.img.getFillColor() );
//    bottomSeg.init( endBall.pos - endBall.r*flipLine.N, centerBall.pos - centerBall.r*flipLine.N, endBall.img.getFillColor() );
}

void Flipper::draw( sf::RenderTarget& rRW )const
{
    centerBall.draw(rRW);
    endBall.draw(rRW);
    topSeg.draw(rRW);
    bottomSeg.draw(rRW);
 //   flipLine.draw(rRW);// temp
}

void Flipper::setPosition( vec2d pos )
{
    if( flipLine.state != 0 ) return;
    vec2d dPos = pos - flipLine.pos;
    flipLine.setPosition( pos );
    centerBall.setPosition( pos );
    endBall.setPosition( endBall.pos + dPos );
    topSeg.setPosition( topSeg.pos + dPos );
    bottomSeg.setPosition( bottomSeg.pos + dPos );
}

    bool Flipper::is_up()const
    {
        return flipLine.is_up();
    }

    void Flipper::init_up()// trigger actions
    {
        flipLine.init_up();
        vec2d RceN( -flipLine.L.y, flipLine.L.x );
        endBall.v = flipLine.rotVel*RceN;
        endBall.is_free = true;
    }

    void Flipper::init_dn()
    {
        flipLine.init_dn();
        vec2d RceN( -flipLine.L.y, flipLine.L.x );
        endBall.v = flipLine.rotVel*RceN;
        endBall.is_free = true;
    }

    void Flipper::set_up()// set state
    {
        flipLine.set_up();
        endBall.setPosition( flipLine.pos + flipLine.L );
        endBall.is_free = false;
        endBall.v *= 0.0f;
        resetEdges();
//        topSeg.init( endBall.pos + endBall.r*flipLine.N, centerBall.pos + centerBall.r*flipLine.N, endBall.img.getFillColor() );
 //       bottomSeg.init( endBall.pos - endBall.r*flipLine.N, centerBall.pos - centerBall.r*flipLine.N, endBall.img.getFillColor() );
    }

    void Flipper::set_dn()
    {
        flipLine.set_dn();
        endBall.setPosition( flipLine.pos + flipLine.L );
        endBall.is_free = false;
        endBall.v *= 0.0f;
        resetEdges();
 //       topSeg.init( centerBall.pos + centerBall.r*flipLine.N, endBall.pos + endBall.r*flipLine.N, endBall.img.getFillColor() );
 //       bottomSeg.init( endBall.pos - endBall.r*flipLine.N, centerBall.pos - centerBall.r*flipLine.N, endBall.img.getFillColor() );
    }

