#include "persPyramid.h"

void persPyramid::init( std::istream& is )
{
    is >> pos.x >> pos.y >> pos.z;
    is >> Nb.x >> Nb.y >> Nb.z;
    is >> b >> h;
    vec3f ofst; is >> ofst.x >> ofst.y >> ofst.z;// to pt[0] from pos
    ofst -= ofst.dot(Nb)*Nb;// remove Nb component
    ofst /= ofst.mag();// normalize
    Rbound = b/sqrtf(3.0f);
    pt[0] = pos + ofst*Rbound;
    pt[1] = pos + Rbound*ofst.rotate_axis( Nb, 2.0944f );
    pt[2] = pos + Rbound*ofst.rotate_axis( Nb, -2.0944f );
    pt[3] = pos + h*Nb;
    // color per face
    unsigned int rd, gn, bu;
    is >> rd >> gn >> bu;// base
    vtx[0].color = vtx[1].color = vtx[2].color = sf::Color(rd,gn,bu);
    is >> rd >> gn >> bu;// side 1
    vtx[3].color = vtx[4].color = vtx[5].color = sf::Color(rd,gn,bu);
    is >> rd >> gn >> bu;// side 2
    vtx[6].color = vtx[7].color = vtx[8].color = sf::Color(rd,gn,bu);
    is >> rd >> gn >> bu;// side 3
    vtx[9].color = vtx[10].color = vtx[11].color = sf::Color(rd,gn,bu);
}

void persPyramid::setPosition( vec3f Pos )
{
    vec3f dPos = Pos - pos;
    pos = Pos;
    for( unsigned int j = 0; j < 4; ++j )
        pt[j] += dPos;

    setVtxPositions();
}

void persPyramid::setVtxPositions()
{
    // base
    vtx[0].position = persPt::get_xyw( pt[0] );
    vtx[1].position = persPt::get_xyw( pt[1] );
    vtx[2].position = persPt::get_xyw( pt[2] );
//    vec3f sep = pos - persPt::camPos;
    vec3f sep = persPt::camPos - pos;
    vec3f v1 = ( pt[2] - pt[0] ).cross( pt[1] - pt[0] );
    doDrawSide[0] = sep.dot( v1 ) > 0.0f;
    // side 1  0,1,3
    vtx[3].position = persPt::get_xyw( pt[0] );
    vtx[4].position = persPt::get_xyw( pt[1] );
    vtx[5].position = persPt::get_xyw( pt[3] );
    v1 = ( pt[3] - pt[0] ).cross( pt[3] - pt[1] );
    doDrawSide[1] = sep.dot( v1 ) > 0.0f;
    // side 2  1,2,3
    vtx[6].position = persPt::get_xyw( pt[1] );
    vtx[7].position = persPt::get_xyw( pt[2] );
    vtx[8].position = persPt::get_xyw( pt[3] );
    v1 = ( pt[3] - pt[1] ).cross( pt[3] - pt[2] );
    doDrawSide[2] = sep.dot( v1 ) > 0.0f;
    // side 3  2,0,3
    vtx[9].position = persPt::get_xyw( pt[2] );
    vtx[10].position = persPt::get_xyw( pt[0] );
    vtx[11].position = persPt::get_xyw( pt[3] );
    v1 = ( pt[3] - pt[2] ).cross( pt[3] - pt[0] );
    doDrawSide[3] = sep.dot( v1 ) > 0.0f;
}

void persPyramid::update( float dt )
{
    if( isMoving )
    {
        setPosition( pos + vel*dt );
    }

    update_doDraw();
    setVtxPositions();
}

void persPyramid::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;

    if( doDrawSide[0] ) RT.draw( &vtx[0], 3, sf::Triangles );
    if( doDrawSide[1] ) RT.draw( &vtx[3], 3, sf::Triangles );
    if( doDrawSide[2] ) RT.draw( &vtx[6], 3, sf::Triangles );
    if( doDrawSide[3] ) RT.draw( &vtx[9], 3, sf::Triangles );
}
