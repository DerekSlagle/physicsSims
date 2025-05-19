#include "persCurve.h"

void persCurve::init( std::istream& is, std::function<void(float,vec3f&)> p_InitFunc )
{
    if( !p_InitFunc ){ std::cout << "\npersCurve() null pInitFunc"; return; }
    pInitFunc = p_InitFunc;

    is >> pos.x >> pos.y >> pos.z;

    is >> Zup.x >> Zup.y >> Zup.z;
    Zup /= Zup.mag();

    // or given yaw and pitch
/*    float yawAngle, pitchAngle; is >> yawAngle >> pitchAngle;
    yawAngle *= vec2f::PI/180.0f;
    pitchAngle *= vec2f::PI/180.0f;
    Xup = persPt::xHat;
    Yup = persPt::yHat;
    Zup = persPt::zHat;
    vec3f::yaw( yawAngle, Xup, Yup, Zup );
    vec3f::pitch( pitchAngle, Xup, Yup, Zup );  */

 //   is >> Rbound;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    is >> paramMin >> paramMax >> rotSense;
    unsigned int N; is >> N;
    ptVec.reserve(N);
    vtxVec.reserve(N);

    //form vector basis with Xup horizontal
    if( Zup.dot( persPt::yHat )*Zup.dot( persPt::yHat ) < 0.9 )// cross with yHat
    {
        Xup = persPt::yHat.cross( Zup );// xu
        Xup /= Xup.mag();
        Yup = Zup.cross( Xup );// yu
    }
    else// cross with xHat
    {
        Yup = Zup.cross( persPt::xHat );// yu
        Yup /= Yup.mag();
        Xup = Yup.cross( Zup );// xu
//        Xup = Zup.cross( Yup );// xu
    }

    sf::Vertex vtxTemp;
    vtxTemp.color = sf::Color(rd,gn,bu);
    vec3f ptTemp;
    float dPar = ( paramMax - paramMin )/(float)( N - 1 );
    for( unsigned int i = 0; i < N; ++i )
    {
        float par = paramMin + (float)i*dPar;
        pInitFunc( par, ptTemp );// writes components in basis Xup, Yup, Zup
    //    if( rotSense == 'R' ){ ptTemp.x *= -1.0f; ptTemp.y *= -1.0f; }// reflect about Yup, Zup plane
        if( rotSense == 'R' ){ ptTemp.x *= -1.0f; ptTemp.z *= -1.0f; }// reflect about Yup, Zup plane
        ptVec.push_back( ptTemp );
        vtxTemp.position = persPt::get_xyw( pos + ptTemp.x*Xup + ptTemp.y*Yup + ptTemp.z*Zup );
        vtxVec.push_back( vtxTemp );
    }

    // Rbound is to furthest pt from pos
    Rbound = ptVec[0].dot( ptVec[0] );// initial value. furthest distance squared sought
    for( unsigned int i = 1; i < N; ++i )
    {
        float rSq = ptVec[i].dot( ptVec[i] );
        if( rSq > Rbound ) Rbound = rSq;
    }
    Rbound = sqrtf( Rbound );
    std::cout << "\n persCurve::init(): Rbound = " << Rbound;

    const float Rcs = 4.0f;
    posCS.setRadius( Rcs );
    posCS.setOrigin( Rcs, Rcs );
    posCS.setFillColor( vtxTemp.color );

    update(0.0f);
    std::cout << "\npersCurve::init(): doDraw = " << doDraw;
}

void persCurve::init( const persCurve& prevCurve, sf::Color color, float parMin, float parMax, unsigned int N, std::function<void(float,vec3f&)> p_InitFunc )
{
    if( !p_InitFunc ){ std::cout << "\npersCurve() null pInitFunc"; return; }
    pInitFunc = p_InitFunc;


    paramMin = parMin;// + overPar;
    paramMax = parMax;// + overPar;

    ptVec.reserve(N);
    vtxVec.reserve(N);
    rotSense = prevCurve.rotSense == 'L' ? 'R' : 'L';

    // must add amount over 90 degrees in prevCurve if prevCurve.rotSense = 'R'
    float overPar = prevCurve.paramMax - 0.5f*vec2f::PI;
    if( rotSense == 'L' )// good
    {
        paramMin += overPar;
        paramMax += overPar;
    }
 //   else// no adjustment needed
 //   {
     //   paramMin += overPar;
     //   paramMax += overPar;
 //   }

    Xup = prevCurve.Xup;
    Yup = prevCurve.Yup;
    Zup = prevCurve.Zup;

    // plot the ptVec[]
    vec3f ptTemp;
    float dPar = ( paramMax - paramMin )/(float)( N - 1 );
    Rbound = 0.0f;// as minimum in search for max
    for( unsigned int i = 0; i < N; ++i )
    {
        float par = paramMin + (float)i*dPar;
        pInitFunc( par, ptTemp );// writes components in basis Xup, Yup, Zup
    //    if( rotSense == 'R' ){ ptTemp.x *= -1.0f; ptTemp.y *= -1.0f; }// reflect about Yup, Zup plane
        if( rotSense == 'R' ){ ptTemp.x *= -1.0f; ptTemp.z *= -1.0f; }// reflect about Yup, Zup plane
        ptVec.push_back( ptTemp );
        float rSq = ptTemp.dot( ptTemp );
        if( rSq > Rbound ) Rbound = rSq;
    }
    Rbound = sqrtf( Rbound );
    std::cout << "\n persCurve::init(): Rbound = " << Rbound;

    // assign pos
    vec3f endPt = rotSense == 'L' ? prevCurve.ptVec.back() : prevCurve.ptVec.front();
    endPt = Xup*endPt.x + Yup*endPt.y + Zup*endPt.z;// to world coordinates
    vec3f ctrOfst = endPt*Rbound/prevCurve.Rbound;
    pos = prevCurve.pos + endPt + ctrOfst;
    // assign vertex positions
    sf::Vertex vtxTemp;
    vtxTemp.color = color;
    for( unsigned int i = 0; i < N; ++i )
    {
        vtxTemp.position = persPt::get_xyw( pos + ptVec[i].x*Xup + ptVec[i].y*Yup + ptVec[i].z*Zup );
        vtxVec.push_back( vtxTemp );
    }
    // circle at center
    const float Rcs = 4.0f;
    posCS.setRadius( Rcs );
    posCS.setOrigin( Rcs, Rcs );
    posCS.setFillColor( vtxTemp.color );

    update(0.0f);
    std::cout << "\npersCurve::init( persCurve& ): doDraw: " << doDraw;
}

void persCurve::constructAxes( vec3f Zu_p )// from given Zup and existing Yup
{
    // construct primed frame
    Zup = Zu_p;
    Zup /= Zup.mag();
    Xup = Yup.cross( Zup );
    Xup /= Xup.mag();
    Yup = Zup.cross( Xup );
}


// current definition here
void persCurve::update_doDraw()
{
    idxBeginA = idxBeginB = 0;
    NvtxA = 0;
    NvtxB = 0;

    // check based on pos and Rbound
    float csA = persPt::camDir.dot( Zup );
    float Rproj = Rbound*sqrt( 1.0f - csA*csA );// projection of Xup, Yup plane towards/away from viewer
    vec3f dPos = pos - persPt::camPos;
    float Uctr = persPt::camDir.dot( dPos );
//    if( Uctr + Rproj < 0.0f ) doDraw = false;
    if( Uctr + Rproj < 0.0f )
    {
    //    if( doDraw ) std::cout << "\ndoDraw: false - behind";
        doDraw = false;
        return;// pos is more than Rproj behind camera
    }
    // check if pos is left or right of window
    float scProj = persPt::xu.dot( dPos );// projection in xu direction
    if( scProj > 0.0f ) scProj -= Rbound;
    else scProj += Rbound;
    scProj *= persPt::Z0/Uctr;
    if( scProj > persPt::X0 || scProj < -persPt::X0 )
    {
    //    if( doDraw ) std::cout << "\ndoDraw: false - left or right";
        doDraw = false;
        return;// pos is lt/rt
    }
    // check if above or below - NOT WORKING
    scProj = persPt::yu.dot( dPos );// projection in yu direction
    if( scProj > 0.0f ) scProj -= Rbound;
    else scProj += Rbound;
    scProj *= persPt::Z0/Uctr;
//    if( scProj > persPt::Yh || scProj < -persPt::Yh ) { if( doDraw ) std::cout << "\ndoDraw: false - above or below"; doDraw = false; return; }// pos is above or below

    bool onA = true;
    bool segmentActive = false;
    bool anyInView = false;// true when 1st found ib window bounds

    // temp old way
 //   persPt::update_doDraw();
 //   if( doDraw ) std::cout << "\n persCurve::doDraw = true";
 //   else std::cout << "\n persCurve::doDraw = false";
 //   NvtxA = vtxVec.size();
 //   return;

    // find which are in front of the camera
    for( unsigned int j = 0; j < ptVec.size(); ++j )
    {
        const vec3f& pt = ptVec[j];
        vec3f sep = pos + pt.x*Xup + pt.y*Yup + pt.z*Zup - persPt::camPos;
        float U = persPt::camDir.dot( sep );
        if( U > 10.0f )// pt is at least 10px in front of camera
        {
            if( !segmentActive )
            {
                if( onA ) idxBeginA = j;
                else idxBeginB = j;
                segmentActive = true;
            }

            if( !anyInView )
            {
                vec3f L = sep - U*persPt::camDir;
                L *= persPt::Z0/U;
                if( L.x < persPt::X0 && L.x > -persPt::X0 &&  L.y < persPt::Yh && L.y > -persPt::Yh )
                    anyInView = true;
            }
        }
        else if( segmentActive )// end is found
        {
            if( onA )
            {
                if( j > 1 + idxBeginA ) NvtxA = j - idxBeginA;
                onA = false;
                segmentActive = false;
            }
            else if( j > 1 + idxBeginB )
            {
                NvtxB = j - idxBeginB;
                break;
            }
        }
    }

    if( segmentActive )
    {
        if( onA ) NvtxA = vtxVec.size() - idxBeginA;
        else NvtxB = vtxVec.size() - idxBeginB;
    }

    bool doDrawLast = doDraw;// temp
    doDraw = anyInView && ( NvtxA > 1 || NvtxB > 1 );

 //   if( doDraw && !doDrawLast ) std::cout << "\ndoDraw: true";
 //   else if( !doDraw && doDrawLast ) std::cout << "\ndoDraw: false";
}


/*
void persCurve::update_doDraw()
{
    idxBeginA = idxBeginB = 0;
    NvtxA = 0;
    NvtxB = 0;
    bool onA = true;
 //   bool segmentActive = false;
    bool anyInView = false;// true when 1st found ib window bounds

    // temp old way
 //   persPt::update_doDraw();
 //   if( doDraw ) std::cout << "\n persCurve::doDraw = true";
 //   else std::cout << "\n persCurve::doDraw = false";
 //   NvtxA = vtxVec.size();
 //   return;

    // find which are in front of the camera
    for( unsigned int j = 0; j < ptVec.size(); ++j )
    {
        const vec3f& pt = ptVec[j];
        vec3f ptPos = pos + pt.x*Xup + pt.y*Yup + pt.z*Zup;
        vec3f sep = ptPos - persPt::camPos;
        float U = persPt::camDir.dot( sep );
        if( U > 10.0f )// pt is at least 10px in front of camera
        {
            vec3f L = sep - U*persPt::camDir;// component perpendicular to camDir
            L *= persPt::Z0/U;// scaling applied

            if( !anyInView )
            {
                if( L.x < persPt::X0 && L.x > -persPt::X0 &&  L.y < persPt::Yh && L.y > -persPt::Yh )
                {
                    anyInView = true;
                    if( onA ) idxBeginA = j;
                    else idxBeginB = j;
                        //     vtxVec[j].position = sf::Vector2f( persPt::X0 + L.dot( persPt::xu ), persPt::Yh - L.dot( persPt::yu ) );
                 //   vtxVec[j].position = persPt::get_xyw( ptPos );
                }

            }
            else// find end of in view segment
            {

                // if within the window
                if( L.x < persPt::X0 && L.x > -persPt::X0 &&  L.y < persPt::Yh && L.y > -persPt::Yh )
                {
                    //     vtxVec[j].position = sf::Vector2f( persPt::X0 + L.dot( persPt::xu ), persPt::Yh - L.dot( persPt::yu ) );
                 //   vtxVec[j].position = persPt::get_xyw( ptPos );
                }
                else
                {
                    anyInView = false;
                    if( onA )
                    {
                        if( j > 1 + idxBeginA ) NvtxA = j - idxBeginA;
                        onA = false;
                    }
                    else if( j > 1 + idxBeginB )
                    {
                        NvtxB = j - idxBeginB;
                        break;
                    }
                }
            }
        }
    }

 //   if( segmentActive )
    if( anyInView )
    {
        if( onA ) NvtxA = vtxVec.size() - idxBeginA;
        else NvtxB = vtxVec.size() - idxBeginB;
    }

    bool doDrawLast = doDraw;// temp
    doDraw = anyInView && ( NvtxA > 1 || NvtxB > 1 );

    if( doDraw && !doDrawLast ) std::cout << "\ndoDraw: true";
    else if( !doDraw && doDrawLast ) std::cout << "\ndoDraw: false";
}
*/

void persCurve::update( float dt )
{
    if( !inUse ) return;
    update_doDraw();
 //   return;

    // TEMP
  //  idxBeginA = idxBeginB = 0;
  //  NvtxA = vtxVec.size();
  //  NvtxB = 0;

    if( doDraw )
    {
     //   size_t N = ptVec.size() < vtxVec.size() ? ptVec.size() : vtxVec.size();// silly because both are same size
     //   for( size_t i = 0; i < N; ++i )
     //       vtxVec[i].position = persPt::get_xyw( pos + ptVec[i].x*Xup + ptVec[i].y*Yup + ptVec[i].z*Zup );

        if( NvtxA > 1 && idxBeginA + NvtxA <= vtxVec.size() )
            for( unsigned int i = idxBeginA; i < idxBeginA + NvtxA; ++i )
                vtxVec[i].position = persPt::get_xyw( pos + ptVec[i].x*Xup + ptVec[i].y*Yup + ptVec[i].z*Zup );

        if( NvtxB > 1 && idxBeginB + NvtxB <= vtxVec.size() )
            for( unsigned int i = idxBeginB; i < idxBeginB + NvtxB; ++i )
                vtxVec[i].position = persPt::get_xyw( pos + ptVec[i].x*Xup + ptVec[i].y*Yup + ptVec[i].z*Zup );

        posCS.setPosition( persPt::get_xyw( pos ) );
    }
}

void persCurve::draw( sf::RenderTarget& RT ) const
{
    if( doDraw )
    {
        if( NvtxA > 1 ) RT.draw( &(vtxVec[idxBeginA]), NvtxA, sf::LinesStrip );
        if( NvtxB > 1 ) RT.draw( &(vtxVec[idxBeginB]), NvtxB, sf::LinesStrip );
        RT.draw( posCS );
    }
}

void persCurve::setPosition( vec3f Pos )
{
    pos = Pos;
    update_doDraw();
    for( size_t i = 0; i < ptVec.size(); ++i )
    {
        if( doDraw ) vtxVec[i].position = persPt::get_xyw( pos + ptVec[i].x*Xup + ptVec[i].y*Yup + ptVec[i].z*Zup );
    }
}
