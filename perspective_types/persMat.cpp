#include "persMat.h"

persMat::persMat()
{
    //ctor
}

persMat::~persMat()
{
    //dtor
}

bool persMat::init( const char* fileName, std::function<float(float,float)> p_Fxz )
{
    std::ifstream fin( fileName );
    if( !fin ) return false;
    init( fin, p_Fxz );
    return true;
}

void persMat::init( std::istream& is, std::function<float(float,float)> p_Fxz )
{
    pFxz = p_Fxz;
    is >> pos.x >> pos.y >> pos.z;
    is >> szX >> szZ;
    Rbound = 0.5f*sqrtf( szX*szX + szZ*szZ );
    is >> nCellsX >> nCellsZ;// plot in y direction
    is >> xMin >> xMax;
    is >> zMin >> zMax;
    scaleX = szX/( xMax - xMin );
    scaleZ = szZ/( zMax - zMin );

    sf::Vertex tempVtx;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    tempVtx.color = sf::Color(rd,gn,bu);
    // plot
    matVec.reserve( nCellsZ*nCellsX );
    float dx = ( xMax - xMin )/nCellsX;
    float dz = ( zMax - zMin )/nCellsZ;
    vec3f origin = pos;
    origin.x -= 0.5f*szX;
    origin.z -= 0.5f*szZ;
 //   float scaleX = szX/nCellsX;
 //   float scaleZ = szZ/nCellsZ;
    float cellSzX = szX/nCellsX;
    float cellSzZ = szZ/nCellsZ;
    yMin = yMax = origin.y + pFxz( xMin, zMin );// at 1st pt visited

    for( unsigned int iz = 0; iz < nCellsZ; ++iz )
        for( unsigned int ix = 0; ix < nCellsX; ++ix )
        {
            float fx = xMin + ix*dx, fz = zMin + iz*dz;
            vec3f ptPos = origin;
            ptPos.x = origin.x + ix*cellSzX;// + ix*scaleX;
            ptPos.y = origin.y + pFxz( fx, fz );
            ptPos.z = origin.z + iz*cellSzZ;// + iz*scaleZ;
            matVec.push_back( ptPos );

            if( ptPos.y > yMax ) yMax = ptPos.y;
            else if( ptPos.y < yMin ) yMin = ptPos.y;
        }

     vtxVec.resize( 2*nCellsX*nCellsZ, tempVtx );// 2 for each point
     update(0.0f);
}

void persMat::setPosition( vec3f Pos )
{
    vec3f dPos = Pos - pos;
    pos = Pos;
    for( vec3f& pt : matVec ) pt += dPos;
}

void persMat::update( float dt )
{
 //   bool doDrawLast = doDraw;
    persPt::update_doDraw();
 //   if( !doDrawLast && doDraw ) std::cout << "\n mat doDraw: true";
 //   else if( doDrawLast && !doDraw ) std::cout << "\n mat doDraw: false";

    if( !doDraw ) return;

    // map sf::vertex to vec3f
    if( vtxVec.size() < 2*nCellsX*nCellsZ ) return;
    if( vtxVec.size() != 2*matVec.size() ) return;
    // lines in x direction match vec3f layout
    for( unsigned int j = 0; j < matVec.size(); ++j )
        vtxVec[j].position = persPt::get_xyw( matVec[j] );

    // lines in z direction trace z columns: matVec[z][x]
    sf::Vertex* pVtx = &vtxVec[nCellsX*nCellsZ];// offset to 2nd half
    for( unsigned int idxX = 0; idxX < nCellsX; ++idxX )
        for( unsigned int idxZ = 0; idxZ < nCellsZ; ++idxZ )
        {
       //     pVtx[ idxZ*nCellsX + idxX ].position = persPt::get_xyw( matVec[ idxZ*nCellsX + idxX ] );
            pVtx[ idxX*nCellsZ + idxZ ].position = persPt::get_xyw( matVec[ idxZ*nCellsX + idxX ] );
        }
}

void persMat::draw( sf::RenderTarget& RT )const
{
    if( !doDraw ) return;
    // lines in x direction
    for( unsigned int idxZ = 0; idxZ < nCellsZ; ++idxZ )
        RT.draw( &( vtxVec[idxZ*nCellsX] ), nCellsX, sf::LinesStrip );
    // lines in z direction
    for( unsigned int idxX = 0; idxX < nCellsX; ++idxX )
        RT.draw( &( vtxVec[ nCellsZ*( nCellsX + idxX ) ] ), nCellsZ, sf::LinesStrip );
}

bool persMat::inBoundingBox( vec3f ptPos )const
{
    if( ptPos.x < pos.x - 0.5f*szX ) return false;// x
    if( ptPos.x > pos.x + 0.5f*szX ) return false;
    if( ptPos.z < pos.z - 0.5f*szZ ) return false;// z
    if( ptPos.z > pos.z + 0.5f*szZ ) return false;
    if( ptPos.y < yMin || ptPos.y > yMax ) return false;// y
    return true;
}

vec3f persMat::getGradient( float x, float z )const
{
    if( pFx && pFz )
        return persPt::yHat - pFx(x,z)*scaleX*persPt::xHat - pFz(x,z)*scaleZ*persPt::zHat;

    // otherwise estimate
    float Fx = 0.0f, Fz = 0.0f;
    getFx_Fy( x, z, Fx, Fz );
    return persPt::yHat - Fx*scaleX*persPt::xHat - Fz*scaleZ*persPt::zHat;

    return vec3f(0.0f,1.23f,0.0f);
}

bool persMat::getFx_Fy( float x, float z, float& Fx, float& Fz )const
{
 //   float xm = x - pos.x + 0.5f*szX, zm = z - pos.z + 0.5f*szZ;// relative to mat origin
 //   float xm = x - xMin, zm = z - zMin;// relative to mat origin
    float xm = x - xMin, zm = z - zMin;// relative to mat origin
    float cellSzX = szX/nCellsX, cellSzZ = szZ/nCellsZ;
    // index to bottom left corner of cell = point P0
    int ix0 = static_cast<int>(xm/cellSzX), iz0 = static_cast<int>(zm/cellSzZ);
    if( ix0 < 0 || ix0 + 1 > static_cast<int>(nCellsX) ) return false;
    if( iz0 < 0 || iz0 + 1 > static_cast<int>(nCellsZ) ) return false;

    float dx = xm - ix0*cellSzX, dz = zm - iz0*cellSzZ;// amount into cell
    // corners clockwise from P0 are P1, P3, P2
    int iP0 = ix0 + iz0*nCellsX;// index in matVec to P0
    int iP2 = iP0 + 1;// bottom right
    int iP1 = iP0 + nCellsX;// top left
    int iP3 = iP1 + 1;// bottom left
    // Fx and Fz at edges
    float Fz_Lt = ( matVec[iP1].y - matVec[iP0].y )/cellSzZ;
    float Fz_Rt = ( matVec[iP3].y - matVec[iP2].y )/cellSzZ;
    Fz = Fz_Lt + dx*( Fz_Rt - Fz_Lt )/cellSzX;

    float Fx_Bott = ( matVec[iP2].y - matVec[iP0].y )/cellSzX;
    float Fx_Top = ( matVec[iP3].y - matVec[iP1].y )/cellSzX;
    Fx = Fx_Bott + dz*( Fx_Top - Fx_Bott )/cellSzZ;

    return true;
}

bool persMat::getFx_Fy_test( std::istream& is, std::ostream& os )const
{
    if( !( pFx && pFz ) ) return false;
    int numDivs; is >> numDivs;
    int ix0, iz0; is >> ix0 >> iz0;
 //   if( ix0 < 0 || ix0 )
    float cellSzX = szX/nCellsX, cellSzZ = szZ/nCellsZ;
    float dx = cellSzX/numDivs, dz = cellSzZ/numDivs;
    float Fx_appx = 0.0f, Fz_appx = 0.0f;

    for( int r = 0; r < numDivs; ++r )
    {
        float Z = cellSzZ*iz0 + r*dz + zMin;
        os << "\nZ = " << Z;
        for( int c = 0; c < numDivs; ++c )
        {
            float X = cellSzX*ix0 + c*dx + xMin;
            if( getFx_Fy( X, Z, Fx_appx, Fz_appx ) )
            {
                float Fx = pFx(X,Z), Fz = pFz(X,Z);
                os << "\nX = " << X << "  " << '\n' << Fx_appx << "  " << Fx << "  " << 100.0f*( Fx_appx - Fx )/Fx;
                os << "  " << '\n' << Fz_appx << "  " << Fz << "  " << 100.0f*( Fz_appx - Fz )/Fz;
            }
            else os << "\n bad get";
        }
    }

    return true;
}

/*
// FAIL
bool persMat::hitMat( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const// no bound box check
{
    vec3f origin = pos - 0.5f*szX*persPt::xHat - 0.5f*szZ*persPt::zHat;
    vec3f L = posB - posA;
    float Lmag = L.mag();
    double xA = ( posA.x - origin.x )/scaleX + xMin, zA = ( posA.z - origin.z )/scaleZ + zMin;
    double xB = ( posB.x - origin.x )/scaleX + xMin, zB = ( posB.z - origin.z )/scaleZ + zMin;

    if( L.dot( persPt::yHat ) < -0.8f*Lmag || L.dot( persPt::yHat ) > 0.8f*Lmag )
    {
        P.x = 0.5f*( posA.x + posB.x );// going with average
        P.z = 0.5f*( posA.z + posB.z );
        P.y = pFxz( 0.5f*( xA + xB ), 0.5f*( zA + zB ) );
        if( ( posA.y - P.y )*( posB.y - P.y ) > 0.0f ) return false;// both on same side of surface
    }
    else
    {
        double dyA = posA.y - origin.y - pFxz( xA, zA );
        double dyB = posB.y - origin.y - pFxz( xB, zB );
        if( dyA*dyB > 0.0 ) return false;// both on same side of surface
        // surface has been crossed
        double dA = dyA > 0.0 ? dyA : -dyA;// need > 0
        double dB = dyB > 0.0 ? dyB : -dyB;
        P = posA + L*( dA/( dA + dB ) );
    }

    //   std::cout << "\npersMat Hit!";
    double xP = xMin + ( xMax - xMin )*( P.x - origin.x )/(szX*scaleX);
    double zP = zMin + ( zMax - zMin )* ( P.z - origin.z )/(szZ*scaleZ);
    vec3f Nu = getGradient( xP, zP );
    Nu /= Nu.mag();// unit normal to surface
    vu = L - 2.0f*( L.dot(Nu) )*Nu;
    vu /= vu.mag();
 //   std::cout << "\n xP = " << xP << " zP = " << zP;
 //   std::cout << "\nP: " << "( " << (P - pos).x << ", " << (P - pos).y << ", " << (P - pos).z << " )";
 //   std::cout << "\nNu: " << "( " << Nu.x << ", " << Nu.y << ", " << Nu.z << " )";
 //   std::cout << "\nFx = " << pFx(xP,zP) << "  Fz = " << pFz(xP,zP);
  //  std::cout << "\ndyA = " << dyA << "  dyB = " << dyB;
    return true;
}
*/

vec3f persMat::toWorldPos( double matPosX, double matPosZ )const
{
    vec3f origin = pos - 0.5f*szX*persPt::xHat - 0.5f*szZ*persPt::zHat;
    vec3f worldPos;
    worldPos.x = ( matPosX - xMin )*scaleX + origin.x;
    worldPos.z = ( matPosZ - zMin )*scaleZ + origin.z;
    worldPos.y = pFxz( matPosX, matPosZ ) + origin.y;
    return worldPos;
}

vec3f persMat::toMatPos( vec3f worldPos )const
{
    vec3f origin = pos - 0.5f*szX*persPt::xHat - 0.5f*szZ*persPt::zHat;
    vec3f matPos;
    matPos.x = ( worldPos.x - origin.x )/scaleX + xMin;
    matPos.z = ( worldPos.z - origin.z )/scaleZ + zMin;
    matPos.y = pFxz( matPos.x, matPos.z );
    return matPos;
}

bool persMat::hitMat( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const// no bound box check
{
    vec3f origin = pos - 0.5f*szX*persPt::xHat - 0.5f*szZ*persPt::zHat;
    double xA = ( posA.x - origin.x )/scaleX + xMin, zA = ( posA.z - origin.z )/scaleZ + zMin;
    double xB = ( posB.x - origin.x )/scaleX + xMin, zB = ( posB.z - origin.z )/scaleZ + zMin;

    double dyA = posA.y - origin.y - pFxz( xA, zA );
    double dyB = posB.y - origin.y - pFxz( xB, zB );
    if( dyA*dyB > 0.0 ) return false;// both on same side of surface
 //   std::cout << "\npersMat Hit!";
    // surface has been crossed
    vec3f L = posB - posA;
    double dA = dyA > 0.0 ? dyA : -dyA;// need > 0
    double dB = dyB > 0.0 ? dyB : -dyB;
    P = posA + L*( dA/( dA + dB ) );
    double xP = xMin + ( xMax - xMin )*( P.x - origin.x )/(szX*scaleX);
    double zP = zMin + ( zMax - zMin )* ( P.z - origin.z )/(szZ*scaleZ);
    vec3f Nu = getGradient( xP, zP );
    Nu /= Nu.mag();// unit normal to surface
  //  vu = L - ( 1.0f + Cr )*( L.dot(Nu) )*Nu;
 //   vu /= vu.mag();

    vu -= ( 1.0f + Cr )*( vu.dot(Nu) )*Nu;

    // set on surface
    if( L.dot( Nu ) < 0.0f ) P += Nu;
    else P -= Nu;

 //   std::cout << "\n xP = " << xP << " zP = " << zP;
 //   std::cout << "\nP: " << "( " << (P - pos).x << ", " << (P - pos).y << ", " << (P - pos).z << " )";
 //   std::cout << "\nNu: " << "( " << Nu.x << ", " << Nu.y << ", " << Nu.z << " )";
 //   std::cout << "\nFx = " << pFx(xP,zP) << "  Fz = " << pFz(xP,zP);
  //  std::cout << "\ndyA = " << dyA << "  dyB = " << dyB;
    return true;
}

// matPos and Nmat are used in calcs
bool persMat::rideMat( persBall& PB, vec3f& matPos, vec3f& Nmat, vec3f Grav, bool onTop, float dt )const
{
    vec3f u1 = Nmat.cross( persPt::yHat ), u2 = u1.cross( Nmat );// basis in tangent plane
    float u1Mag = u1.mag();
    if( u1Mag < 0.1f )
    {
        u1 = persPt::xHat.cross( Nmat );
        u1 /= u1.mag();
        u2 = Nmat.cross( u1 );
        u2 /= u2.mag();
    }
    else
    {
        u1 /= u1.mag();
        u2 /= u2.mag();
    }

    vec3f vPlane = u1*( PB.vel.dot(u1) ) + u2*( PB.vel.dot(u2) );
    matPos.x += vPlane.x*dt/scaleX;
    matPos.z += vPlane.z*dt/scaleZ;
    vPlane += ( Grav.dot(u1)*dt )*u1 + ( Grav.dot(u2)*dt )*u2;
    PB.vel = vPlane;
    matPos.y = pFxz( matPos.x, matPos.z );
    Nmat = getGradient( matPos.x, matPos.z );
    Nmat /= Nmat.mag();
    PB.pos = toWorldPos( matPos.x, matPos.z ) + Nmat*( onTop ? PB.Rbound : -PB.Rbound );
    PB.setPosition( PB.pos );

    return true;
}

/*
// matPos and Nmat are used in calcs
bool persMat::rideMat( persBall& PB, vec3f& matPos, vec3f& Nmat, vec3f Grav, bool onTop, float dt )const
{
    vec3f origin = pos - 0.5f*szX*persPt::xHat - 0.5f*szZ*persPt::zHat;
    double xMat = ( matPos.x - origin.x )/scaleX + xMin, zMat = ( matPos.z - origin.z )/scaleZ + zMin;
    vec3f u1 = Nmat.cross( persPt::yHat ), u2 = u1.cross( Nmat );// basis in tangent plane
    float u1Mag = u1.mag();
    if( u1Mag < 0.1f )
    {
        u1 = persPt::xHat.cross( Nmat );
        u1 /= u1.mag();
        u2 = Nmat.cross( u1 );
        u2 /= u2.mag();
    }
    else
    {
        u1 /= u1.mag();
        u2 /= u2.mag();
    }

    vec3f vPlane = u1*( PB.vel.dot(u1) ) + u2*( PB.vel.dot(u2) );
    matPos += vPlane*dt;
    vPlane += ( Grav.dot(u1)*dt )*u1 + ( Grav.dot(u2)*dt )*u2;
    PB.vel = vPlane;
    xMat = ( matPos.x - origin.x )/scaleX + xMin;
    zMat = ( matPos.z - origin.z )/scaleZ + zMin;
    matPos.y = pFxz( xMat, zMat );
    Nmat = getGradient( xMat, zMat );
    Nmat /= Nmat.mag();
    PB.pos = matPos + Nmat*( onTop ? PB.Rbound : -PB.Rbound );

    return true;
}
*/

// helper for below
void vec3f_os( vec3f V, const char* Vname, std::ostream& os, bool newLineAfter = false )
{
    os << Vname << " = ( " << V.x << ", " << V.y << ", " << V.z << " )";
    if( newLineAfter ) os << '\n';
}

void persMat::hitTest_test( std::istream& is, std::ostream& os )const
{
    if( !( pFxz && pFx && pFz ) ) return;

    // read in a test point with xMin <= x <= xMax and for z
    double xt, yt, zt; is >> xt >> zt;// mat coordinates
    yt = pFxz( xt, zt );// unscaled

    vec3f origin = pos - 0.5f*szX*persPt::xHat - 0.5f*szZ*persPt::zHat;// <--> ( xMin, 0, zMin ) on mat
    vec3f Pw;// world coordinates of test point
    Pw.y = origin.y + yt;
    Pw.x = origin.x + scaleX*( xt - xMin );
    Pw.z = origin.z + scaleZ*( zt - zMin );

    // generate hit() args posA and posB
    // choose a direction through Pw
    vec3f U; is >> U.x  >> U.y  >> U.z;
    U /= U.mag();// normalize
    // distance along U to posA and posB from Pw
    double a, b; is >> a >> b;// Note: a*b < 0 crosses surface
    vec3f posA = Pw + a*U, posB = Pw + b*U;
    double xa, za, xb, zb;// under posA, posB on  mat
    xa = xMin + ( posA.x - origin.x )/scaleX;
    za = zMin + ( posA.z - origin.z )/scaleZ;
    xb = xMin + ( posB.x - origin.x )/scaleX;
    zb = zMin + ( posB.z - origin.z )/scaleZ;


    // Find expected values. hit() to supply comparisons
    double Fxt = pFx( xt, zt ), Fzt = pFz( xt, zt );// partial derivs at test point
    vec3f Nsurf = getGradient( xt, zt );
    Nsurf /= Nsurf.mag();// normalize
    double dyA = posA.y - pFxz( xa, za ), dyB = posB.y - pFxz( xb, zb );// height above surface at A, B

    os << "\n*** hitTest_test ***";
    os << "\n yt = " << yt << "  Fxt: " << Fxt << "  Fzt: " << Fzt;
    vec3f_os( Pw, "Pw", std::cout, true );
    vec3f_os( Nsurf, "Nsurf", std::cout, true );
    vec3f_os( posA, "posA", std::cout, true );
    vec3f_os( posB, "posB", std::cout, true );
    os << "\n xa: " << xa << " za: " << za <<"   xb: " << xb <<" zb: " << zb;
    os << "\n dyA: " << dyA << "  dyB: " << dyB;

    // call hit()
    vec3f Parg, Varg;
    // should get: Parg = Pw, Varg = U - 2.0*( Nsurf.dot(U) )*U
    if( hitMat( posA, posB, Parg, Varg ) )
    {

        vec3f_os( Parg, "Parg", std::cout, true );
        vec3f_os( Varg, "Varg", std::cout, true );
        vec3f_os( U - 2.0*( Nsurf.dot(U) )*Nsurf, "Vsbe", std::cout, true );
    }
    else std::cout << "\n No Hit!";

    return;// habit
}

bool persMat::hitBoundBox( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const
{
    P = posA;// temp approx
    vec3f t1 = posB - posA;
    t1 /= t1.mag();// unit vector in direction of motion

    // posB is in box and posA is still outside of box
    if( ( posA.x < pos.x - 0.5f*szX ) || ( posA.x > pos.x + 0.5f*szX ) )
        { vu = t1 - 2.0f*( t1.dot( persPt::xHat )*persPt::xHat ); return true; }

    if( ( posA.z < pos.z - 0.5f*szZ ) || ( posA.z > pos.z + 0.5f*szZ ) )
        { vu = t1 - 2.0f*( t1.dot( persPt::zHat )*persPt::zHat ); return true; }

    if( ( posA.y < yMin ) || ( posA.y > yMax ) )
        { vu = t1 - 2.0f*( t1.dot( persPt::yHat )*persPt::yHat ); return true; }

    return false;
}

bool persMat::hit( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const
{
    // bounding box check - nfg lets shots through
    if( !inBoundingBox( posB ) ) return false;
 //   return hitBoundBox( posA, posB, P, vu );
    return hitMat( posA, posB, P, vu );// fail. no hits

 //   return false;
}

// of object on surface. returns position.
// NEAR FAIL! Barely works. r1 must be adjusted downward after most calls or Etot rises constantly
vec3f persMat::update_polarMotion(  float Lz, float r2, float& r, float& r1, float& angPol, float dt, unsigned int numReps )
{
    dt /= numReps;
    for( unsigned int j = 0; j < numReps; ++j )
    {
        float angle1 = Lz/(r*r);
        r += r1*dt + 0.5f*r2*dt*dt;
        r1 += r2*dt;
    //    r1 *= 0.999f;// attempt to correct too high value. result: Too much. Now Etot dives
        angPol += angle1*dt;
    //    float angle1 = Lz/(r*r);
    //    angPol += angle1*dt;
    }

    // assign pos and vel
    vec3f Pos = pos;
    float rx = r*cosf( angPol ), rz = r*sinf( angPol );
    Pos.x += rx*scaleX;
    Pos.z += rz*scaleZ;
    Pos.y += pFxz( rx, rz );
    return Pos;
}

// mattAdapterPolar
void mattAdapterPolar::init( std::istream& is, persMat& rPM, std::function<double(double,double,double)> F_r2, persPt& rRider )
{
    is >> r0 >> r10 >> ang0 >> ang10;
    is >> numReps;
    init( r0, r10, ang0, ang10, rPM, F_r2, rRider );
}

void mattAdapterPolar::init( double R0, double R10, double Ang0, double Ang10, persMat& rPM, std::function<double(double,double,double)> F_r2, persPt& rRider )
{
    f_r2 = F_r2;
    pRider = &rRider;
    pPM = &rPM;
    r = r0 = R0;
    r1 = r10 = R10;
    ang = ang0 = Ang0;
    ang1 = ang10 = Ang10;
    Ly = r*r*ang1;
}

void mattAdapterPolar::setPosition()const
{
    if( !pRider ) return;// maybe caller assigns?
    vec3f Pos = pPM->pos;
    float rx = static_cast<float>( r*cos( ang ) ), rz = static_cast<float>( r*sin( ang ) );
    Pos.x += rx*pPM->scaleX;
    Pos.z += rz*pPM->scaleZ;
    Pos.y += pPM->pFxz( rx, rz );
    pRider->setPosition( Pos );
}

void mattAdapterPolar::reset()// to initial conditions
{
    r = r0; r1 = r10;
    ang = ang0; ang1 = ang10;
    Ly = r*r*ang1;
    setPosition();
}

void mattAdapterPolar::update( float dt )
{
    if( !pPM )return;
//    if( !pPM->isMoving )
    if( !doAnimate )
    {
        if( pRider ) pRider->update(dt);
        return;// to pause animation assign doAnimate = false
    }

    dt /= numReps;


    for( unsigned int j = 0; j < numReps; ++j )
    {
        double r2 = f_r2( r, r1, ang1 );
        ang1 = Ly/(r*r);
        r += r1*dt + 0.5f*r2*dt*dt;
        r1 += r2*dt;
    //    ang1 = Ly/(r*r);
        ang += ang1*dt;
    }

    // assign pos
    setPosition();
}

// matAdapter2ndOrder
void matAdapter2ndOrder::init( std::istream& is, persMat& rPM, std::function<double(double,double)> F_r2, persPt& rRider, double* p_Z )
{
    is >> x0 >> x10;
    is >> numReps;
    init( x0, x10, rPM, F_r2, rRider, p_Z );
}

void matAdapter2ndOrder::init( double X0, double X10, persMat& rPM, std::function<double(double,double)> F_r2, persPt& rRider, double* p_Z )
{
    f_r2 = F_r2;
    pRider = &rRider;
    pZ = p_Z;
    pPM = &rPM;
    pf = pf1 = pf2 = nullptr;
    x = x0 = X0;
    x1 = x10 = X10;
}

// using f, f1, f2
void matAdapter2ndOrder::init( double X0, double X10, persMat& rPM, persPt& rRider, std::function<double(double)> p_f, std::function<double(double)> p_f1, std::function<double(double)> p_f2, double* p_Z )
{
    f_r2 = nullptr;
    pRider = &rRider;
    pZ = p_Z;
    pPM = &rPM;
    pf = p_f;
    pf1 = p_f1;
    pf2 = p_f2;
    x = x0 = X0;
    x1 = x10 = X10;
    usef_r2 = false;
}

void matAdapter2ndOrder::update( float dt )
{
    if( !pPM )return;
    if( !doAnimate )
    {
        if( pRider ) pRider->update(dt);
        return;// to pause animation assign doAnimate = false
    }

    dt /= numReps;
    if( usef_r2 && f_r2 )
    {
        for( unsigned int j = 0; j < numReps; ++j )
        {
            double x2 = f_r2( x, x1 );
            x += x1*dt + 0.5f*x2*dt*dt;
            x1 += x2*dt;
        }
    }
    else if( pf && pf1 && pf2 )
    {
        for( unsigned int j = 0; j < numReps; ++j )
        {
            double x2 = pf1(x)*( Grav - x1*x1*pf2(x) )/( 1.0 + pf1(x)*pf1(x) );
            x += x1*dt + 0.5f*x2*dt*dt;
            x1 += x2*dt;
        }
    }

    // assign pos
    setPosition();
}

void matAdapter2ndOrder::reset()// to initial conditions
{
    x = x0; x1 = x10;
    setPosition();
}

void matAdapter2ndOrder::reset( double Z0 )// to initial conditions
{
    x = x0; x1 = x10;
    if( pZ ) *pZ = Z0;
    setPosition();
}

void matAdapter2ndOrder::setPosition()const// using dms and pSetPosition
{
    if( !pRider ) return;// maybe caller assigns?
    vec3f Pos = pPM->pos;
    float rx = static_cast<float>( x ), rz = Pos.z;
    Pos.x += rx*pPM->scaleX;
    if( pZ ) rz = static_cast<float>( *pZ );
    Pos.z += pPM->scaleZ*rz;
    Pos.y += pPM->pFxz( rx, rz );
    pRider->setPosition( Pos );
}

vec3f matAdapter2ndOrder::getVelocity( double Vz )const
{
    const double dx = 0.01*pPM->szX;
    double dt = dx/x1;
 //   double dz =  Vz*dt;
    double dy = pPM->pFxz( x + dx, *pZ + Vz*dt ) - pPM->pFxz( x, *pZ );
    return vec3f( static_cast<float>(x1), static_cast<float>(dy/dt), static_cast<float>(Vz) );
}

// matAdapter2ndOrder_xz
void matAdapter2ndOrder_xz::init( std::istream& is, persMat& rPM, persPt& rRider )
{
    is >> x0 >> x10 >> z0 >> z10;
    is >> numReps;
    init( x0, x10, z0, z10, rPM, rRider );
}

void matAdapter2ndOrder_xz::init( double X0, double X10, double Z0, double Z10, persMat& rPM, persPt& rRider )// assign pFs seperately
{
    pRider = &rRider;
    pPM = &rPM;
    x = x0 = X0;
    x1 = x10 = X10;
    z = z0 = Z0;
    z1 = z10 = Z10;
    rRider.isMoving = false;// adapter assigns position
    std::cout << "\n scaleX = " << rPM.scaleX << "  scaleZ = " << rPM.scaleZ;
}

//void matAdapter2ndOrder_xz::setFxx( std::function<double(double,double)> Fxx )
//{ pFxx = Fxx; }

//void matAdapter2ndOrder_xz::setFzz( std::function<double(double,double)> Fzz )
//{ pFzz = Fzz; }

void matAdapter2ndOrder_xz::update( float dt )
{
    if( !pPM )return;
  //  if( !doAnimate )
  //  {
  //      if( pRider ) pRider->update(dt);
  //      return;// to pause animation assign doAnimate = false
  //  }

    // move under Grav only if rider leaves map area
    if( pRider )
    {
        if( !doAnimate )
        {
            pRider->update(dt);
            return;// to pause animation assign doAnimate = false
        }
        // is animating
        if( !pRider->isMoving )// trigger is moving off of map
        {
            if( ( x < pPM->xMin || x > pPM->xMax ) || ( z < pPM->zMin || z > pPM->zMax ) )
            {
                pRider->vel = getVelocity();
                pRider->isMoving = true;// under gravity alone now
            }
        }

        if( pRider->isMoving )
        {
            pRider->vel.y += Grav*dt;
            pRider->update(dt);
            return;
        }
    }

    dt /= numReps;

    if( pPM->pFx && pFxx )
    {
        for( unsigned int j = 0; j < numReps; ++j )
        {
            double df_dx = pPM->pFx(x,z);
            double x2 = df_dx*( Grav - x1*x1*pFxx(x,z) )/( 1.0 + df_dx*df_dx );
            x += x1*dt + 0.5f*x2*dt*dt;
            x1 += x2*dt;
        }
    }

    if( pPM->pFz && pFzz )
    {
        for( unsigned int j = 0; j < numReps; ++j )
        {
            double df_dz = pPM->pFz(x,z);
            double z2 = df_dz*( Grav - z1*z1*pFzz(x,z) )/( 1.0 + df_dz*df_dz );
            z += z1*dt + 0.5f*z2*dt*dt;
            z1 += z2*dt;
        }
    }

    // assign pos
    setPosition();
}

void matAdapter2ndOrder_xz::reset()// to initial conditions
{
    x = x0; x1 = x10;
    z = z0; z1 = z10;
    setPosition();
    if( pRider ) pRider->isMoving = false;
}

void matAdapter2ndOrder_xz::setPosition()const// using dms and pSetPosition
{
    if( !pRider ) return;// maybe caller assigns?
    vec3f Pos = pPM->pos;// at center
    float rx = static_cast<float>( x ), rz = static_cast<float>( z );
    Pos.x += rx*pPM->scaleX;
    Pos.z += rz*pPM->scaleZ;
    Pos.y += pPM->pFxz( rx, rz );
    pRider->setPosition( Pos );
}

vec3f matAdapter2ndOrder_xz::getVelocity()const
{
    if( !( pPM->pFx && pPM->pFz ) ) return vec3f();
    double y1 = pPM->pFx(x,z)*x1 + pPM->pFz(x,z)*z1;
    return vec3f( static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(z1) );
}

vec3f matAdapter2ndOrder_xz::getGradient( float x, float z )const
{
    if( pPM && pPM->pFx && pPM->pFz )
        return persPt::yHat - pPM->pFx(x,z)*persPt::xHat - pPM->pFz(x,z)*persPt::zHat;

    return vec3f(0.0f,1.0f,0.0f);
}
