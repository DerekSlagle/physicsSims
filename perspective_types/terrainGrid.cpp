#include "terrainGrid.h"

void terrainGrid::init( std::istream& is, std::function<float(float,float)> Yground )
{
    if( !inUse ) return;

    Yg = Yground;

    is >> origin.x >> origin.z;
    origin.y = 0.0f;
    is >> Ag >> dXg >> dZg;
    Bg = Ag*persPt::X0/persPt::Z0;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    gridColor = sf::Color(rd,gn,bu);
    // # of LinesStrips in each direction = max(Ag, 2*Bg)/dXg or dZg
    maxDim = Ag > 2.0f*Bg ? Ag : 2.0f*Bg;
    if( maxDim/dXg <= 1000.0f &&  maxDim/dZg <= 1000.0f )
    {
   //     xVAvec.resize( maxDim/dXg + 2 );// extend past by 1 each way?
        xVAvec.resize( maxDim/dXg + 3 );// extend past by 1 each way?
        for( sf::VertexArray& VA : xVAvec )
        {
            VA.resize( maxDim/dXg + 3 );
            VA.setPrimitiveType( sf::LinesStrip );
        }

        zVAvec.resize( maxDim/dZg + 3 );// extend past by 1 each way?
        for( sf::VertexArray& VA : zVAvec )
        {
        //    VA.resize( maxDim/dZg + 2 );
            VA.resize( maxDim/dZg + 3 );
            VA.setPrimitiveType( sf::LinesStrip );
        }

        update();
    }
    else
    {
        std::cout << "\n terrainGrid::init(): maxDim/dXg = " << maxDim/dXg;
        std::cout << "\n maxDim/dZg = " << maxDim/dZg;
    }
}

void terrainGrid::update()
{
    if( !inUse ) return;
    // h1 = horizontal unit vector in direction of view
    h1 = persPt::camDir - ( persPt::camDir.dot( persPt::yHat ) )*persPt::yHat;
    h1 /= h1.mag();
    h2 = persPt::yHat.cross( h1 );// horizontal unit vector to right
    h2 /= h2.mag();
    PgLt = persPt::camPos + Ag*h1 - Bg*h2 - persPt::camPos.y*persPt::yHat;
    PgRt = PgLt + 2.0f*Bg*h2;
    const vec3f& Pc = persPt::camPos;

    float xMin = PgLt.x < PgRt.x ? PgLt.x : PgRt.x;
    xMin = Pc.x < xMin ? Pc.x : xMin;
    float xMax = PgLt.x > PgRt.x ? PgLt.x : PgRt.x;
    xMax = Pc.x > xMax ? Pc.x : xMax;
    // min and max in z
    float zMin = PgLt.z < PgRt.z ? PgLt.z : PgRt.z;
    zMin = Pc.z < zMin ? Pc.z : zMin;
    float zMax = PgLt.z > PgRt.z ? PgLt.z : PgRt.z;
    zMax = Pc.z > zMax ? Pc.z : zMax;
    // assign grid


  //  int nz0 = ( zMin - origin.z )/dZg - 1, nzf = ( zMax - origin.z )/dZg + 1;
    int nz0 = ( zMin - origin.z )/dZg, nzf = ( zMax - origin.z )/dZg;


    sf::Vertex vtx;
    vtx.color = gridColor;

    // LinesStrips in z direction
    for( sf::VertexArray& VA : zVAvec ) VA.clear();
//    int nx0 = ( xMin - origin.x )/dXg - 1, nxf = ( xMax - origin.x )/dXg + 1;
    int nx0 = ( xMin - origin.x )/dXg, nxf = ( xMax - origin.x )/dXg;
    if( nxf - nx0 + 1 < (int)zVAvec.capacity() )
    {
        zVAvec.resize( nxf - nx0 + 1 );// # of LinesStrips
        for( int ix = nx0; ix <= nxf; ++ix )
        {
         //   zVAvec[ ix - ix0 ].resize(0);
            for( int iz = nz0; iz <= nzf; ++iz )
            {
                vec3f P = origin + ix*dXg*persPt::xHat + iz*dZg*persPt::zHat;
                if( Yg ) P += Yg( origin.x + ix*dXg, origin.z + iz*dZg )*persPt::yHat;
                if( persPt::camDir.dot( P - Pc ) > 0.0f )
                {
                    vtx.position = persPt::get_xyw(P);
                    zVAvec[ ix - nx0 ].append( vtx );
                }
            }
        }
    }
    else std::cout << "\n terrainGrid::update(): zVAvec.cap = " << zVAvec.capacity() << " nxf - nx0 + 1 = " << nxf - nx0 + 1;

    // LinesStrips in x direction
    for( sf::VertexArray& VA : xVAvec ) VA.clear();
 //   if( nzf - nz0 + 1 < (int)xVAvec.capacity() )
    if( nzf - nz0 + 1 < (int)xVAvec.capacity() )
    {
        xVAvec.resize( nzf - nz0 + 1 );
        for( int iz = nz0; iz <= nzf; ++iz )
        {
         //   xVAvec[ iz - iz0 ].resize(0);
            for( int ix = nx0; ix <= nxf; ++ix )
            {
                vec3f P = origin + ix*dXg*persPt::xHat + iz*dZg*persPt::zHat;
                if( Yg ) P += Yg( origin.x + ix*dXg, origin.z + iz*dZg )*persPt::yHat;
                if( persPt::camDir.dot( P - Pc ) > 0.0f )
                {
                    vtx.position = persPt::get_xyw(P);
                    xVAvec[ iz - nz0 ].append( vtx );
                }
            }
        }
    }
    else std::cout << "\n terrainGrid::update(): xVAvec.cap = " << xVAvec.capacity() << " nzf - nz0 + 1 = " << nzf - nz0 + 1;
}

void terrainGrid::draw( sf::RenderTarget& RT ) const
{
    if( !inUse ) return;
    for( const sf::VertexArray& VA : xVAvec ) RT.draw( VA );
    for( const sf::VertexArray& VA : zVAvec ) RT.draw( VA );
}

bool doDrawPoint( vec3f P )
{
    vec3f sep = P - persPt::camPos;
    float U = persPt::camDir.dot( sep );
    if( U < 0.0f ) return false;
    vec3f sepPerp = sep - U*persPt::camDir;
    float proj = sepPerp.x*persPt::Z0/U;
    if( proj < -persPt::X0 ) return false;
    if( proj > persPt::X0 ) return false;
    proj = sepPerp.y*persPt::Z0/U;
    if( proj < -persPt::Yh ) return false;
    if( proj > persPt::Yh ) return false;

    return true;
}

/*

void terrainGrid::mapCorners( vec3f& Pa, vec3f& P0, vec3f& Pf, vec3f& uHat, vec3f& vHat )const
{
    // find apex where uHat splits the angle
    // at Pc = origin
    if( ( uHat.cross(PgLt-origin) ).dot( uHat.cross(PgRt-origin) ) > 0.0f )
    {
        Pa = PgA;
        P0 = PgLt;
        Pf = PgRt;
        if( uHat.dot(h1) < 0.0f ) uHat *= -1.0f;
    }// at PgLt
    else if( ( uHat.cross(PgRt-PgLt) ).dot( uHat.cross(origin-PgLt) ) > 0.0f )
    {
        Pa = PgLt;
        P0 = PgA;
        Pf = PgRt;
        if( uHat.dot(h2) < 0.0f ) uHat *= -1.0f;
    }
    else// at PgRt
    {
        Pa = PgRt;
        P0 = PgA;
        Pf = PgLt;
        if( uHat.dot(h2) > 0.0f ) uHat *= -1.0f;
    }

    vHat = persPt::yHat.cross( uHat );
    if( vHat.dot( Pf - P0 ) < 0.0f ) vHat *= -1.0f;
}
*/
