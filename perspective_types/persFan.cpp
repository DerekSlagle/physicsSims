#include "persFan.h"

std::vector<sf::VertexArray> persFan::shadowVtxArray;// storage for each instance shadow

bool persFan::init( const char* fName, spriteSheet* p_SS )
{
    std::ifstream fin( fName );
    if( !fin ) { std::cout << "\nNo fan data file"; return false; }
    init( fin, p_SS );
    return true;
}

void persFan::init( std::istream& is, spriteSheet* p_SS )
{
    isCircle = true;

    is >> Rbound >> pos.x >> pos.y >> pos.z;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    fanColor = sf::Color(rd,gn,bu);
    shadowColor = sf::Color(0,0,0,100);

    is >> numPoints;

    vtxVec.resize( numPoints + 2 );
    for( sf::Vertex& V : vtxVec ) V.color = fanColor;

    // read in components
 //   is >> Yup.x >> Yup.y >> Yup.z >> Zup.x >> Zup.y >> Zup.z;
 //   Zup /= Zup.mag();
 //   Yup /= Yup.mag();
 //   constructAxes( Zup );

    // or given yaw and pitch
    float yawAngle, pitchAngle; is >> yawAngle >> pitchAngle;
    Xup = persPt::xHat;
    Yup = persPt::yHat;
    Zup = persPt::zHat;
    yaw( yawAngle );
    pitch( pitchAngle );

    xfVec.resize( numPoints );
    yfVec.resize( numPoints );

 //   is >> isCircle;
    if( isCircle )
        initCircle();
    else// read in all numPoints xf and xy values
    {
        for( unsigned int j = 0; j < numPoints; ++j )
            is >> xfVec[j] >> yfVec[j];
    }

    pSS = p_SS;
 //   Zup_Zu_last = persPt::camDir.dot( Zup );
    vec3f sep = pos - persPt::camPos;
    Zup_sep_last = sep.dot( Zup );

    if( pSS && pSS->getFrCount(0) > 1 )// fan has 2 sides
    {
        if( Zup_sep_last >= 0.0f )
            assignTexCoords( false );
        else
            assignTexCoords( true );
    }
    else assignTexCoords( true );

 //   assignTexCoords( persPt::camDir.dot(Zup) > 0.0f );
    graphCircle();
}

// fan is a polar function with Rbound = maximum radius
bool persFan::init( const char* fName, std::function<float(float)> R, spriteSheet* p_SS )// open file then call below
{
    std::ifstream fin( fName );
    if( !fin ) { std::cout << "\nNo fan data file"; return false; }
    init( fin, R, p_SS );
    return true;
}

void persFan::init( std::istream& is, std::function<float(float)> R, spriteSheet* p_SS )
{
    isCircle = false;// radius varies

    is >> pos.x >> pos.y >> pos.z;// Rbound will be found in initCircle(R)
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    fanColor = sf::Color(rd,gn,bu);
    shadowColor = sf::Color(0,0,0,100);
    is >> numPoints;

    vtxVec.resize( numPoints + 2 );
    for( sf::Vertex& V : vtxVec ) V.color = fanColor;

    // or given yaw and pitch
    float yawAngle, pitchAngle; is >> yawAngle >> pitchAngle;
    yawAngle *= vec2f::PI/180.0f;
    pitchAngle *= vec2f::PI/180.0f;
    Xup = persPt::xHat;
    Yup = persPt::yHat;
    Zup = persPt::zHat;
    yaw( yawAngle );
    pitch( pitchAngle );

    xfVec.resize( numPoints );
    yfVec.resize( numPoints );
    initCircle(R);

    pSS = p_SS;
//    Zup_Zu_last = persPt::camDir.dot( Zup );
    vec3f sep = pos - persPt::camPos;
    Zup_sep_last = sep.dot( Zup );

    if( pSS && pSS->getFrCount(0) > 1 )// fan has 2 sides
    {
        if( Zup_sep_last >= 0.0f )
            assignTexCoords( false );
        else
            assignTexCoords( true );
    }
    else assignTexCoords( true );

//    assignTexCoords( sep.dot(Zup) > 0.0f );// works in periphery of view
    graphCircle();
}

persFan::~persFan()
{
}

void persFan::initPerim( sf::Color PerimColor )// call after init()
{
    perimColor = PerimColor;

    perimVec.resize( numPoints + 1 );
    for( unsigned int j = 0; j < numPoints; ++j )
    {
        perimVec[j].color = perimColor;
        perimVec[j].position = persPt::get_xyw( pos + xfVec[j]*Xup + yfVec[j]*Yup );
    }

    perimVec[numPoints] = perimVec[0];// closing strip
}

void persFan::setPosition( vec3f Pos )
{
 //   pos = Pos;
 //   graphCircle();
 //   update_doDraw();

    pos = Pos;
    update_doDraw();
    if( doDraw ) updateCircle();
}

void persFan::update( float dt )
{
    if( !inUse ) return;
    if( isMoving ) pos += vel*dt;

//    bool doDrawLast = doDraw;
    persPt::update_doDraw();
//    if( !doDrawLast && doDraw ) std::cout << "\n fan doDraw: true";
//    else if( doDrawLast && !doDraw ) std::cout << "\n fan doDraw: false";

    if( !doDraw ) return;

    vec3f sep = pos - persPt::camPos;
 //   float Zup_Zu = persPt::camDir.dot( Zup );
    float Zup_sep = sep.dot( Zup );

    if( pSS && pSS->getFrCount(0) > 1 )// fan has 2 sides
    {
        if( Zup_sep_last >= 0.0f && Zup_sep < 0.0f )
            assignTexCoords( false );
        else if( Zup_sep_last < 0.0f && Zup_sep >= 0.0f )
            assignTexCoords( true );
    }

 //   graphCircle();
    updateCircle();

 //   Zup_Zu_last = Zup_Zu;// for next time
    Zup_sep_last = Zup_sep;// for next time
}

void persFan::update( vec3f rotAxis, float rotSpeed, float dt )// spinning
{
    if( !inUse ) return;
    Zup = Zup.rotate_axis( rotAxis, rotSpeed*dt );
    Yup = Yup.rotate_axis( rotAxis, rotSpeed*dt );
    constructAxes( Zup );
    update(dt);
}

// lightDir is towards subject
void persFan::updateShading( vec3f lightDir )// vertex.color assigned
{
 //   const unsigned int maxIdx = 255, minIdx = 80;// delIdx = maxIdx - minIdx;
 //   const unsigned int midIdx = ( minIdx + maxIdx )/2;

    float minF = 0.6f, maxF = 1.0f;
    vec3f sep = pos - persPt::camPos;
    float sepZ = sep.dot(Zup), ltZ = lightDir.dot(Zup);
    float colorF = 1.0f;// full

    // view is of
    if( sepZ*ltZ > 0.0f )// lit side
    {
        if( ltZ < 0.0f ) ltZ *= -1.0f;// Zup may be facing toward or away from lit side
        colorF = minF + ltZ*( maxF - minF );
    }
    else// shaded side
    {
        colorF = minF;
    }

    // fan color
    for( sf::Vertex& vtx : vtxVec )
    {
        vtx.color.r = static_cast<uint8_t>( colorF*static_cast<float>(fanColor.r) );
        vtx.color.g = static_cast<uint8_t>( colorF*static_cast<float>(fanColor.g) );
        vtx.color.b = static_cast<uint8_t>( colorF*static_cast<float>(fanColor.b) );
    }

    if( !perimVec.empty() )
    {
        for( sf::Vertex& vtx : perimVec )
        {
            vtx.color.r = static_cast<uint8_t>( colorF*static_cast<float>(perimColor.r) );
            vtx.color.g = static_cast<uint8_t>( colorF*static_cast<float>(perimColor.g) );
            vtx.color.b = static_cast<uint8_t>( colorF*static_cast<float>(perimColor.b) );
        }
    }

}

void persFan::addShadow()// push_back a VertexArray into the static vector above
{
    sf::VertexArray shadowVA( sf::TriangleFan, numPoints + 2 );
//    shadowColor = sf::Color(100,100,100,100);
    for( unsigned int n = 0; n < shadowVA.getVertexCount(); ++n )
        shadowVA[n].color = shadowColor;

    if( shadowVtxArray.size() < shadowVtxArray.capacity() )
    {
        shadowVtxArray.push_back( shadowVA );
        pShadowVA = &shadowVtxArray.back();// assure capacity reserved for all beforehand
    }
    else
        pShadowVA = nullptr;
}

void persFan::updateShadow( vec3f lightDir, vec3f Nsurf, float yGround )
{
    if( !pShadowVA ) return;
    if( pShadowVA->getVertexCount() != 2 + numPoints ) return;
    doDrawShadow = true;// assumed until found false below

    // ray casting
    float LtN = lightDir.dot( Nsurf );

    // center 1st
    vec3f P = pos;// + xfVec[0]*Xup + yfVec[0]*Yup;
    float s = ( yGround - P.dot( Nsurf ) )/LtN;
    vec3f Pg = P + lightDir*s;
    float U = persPt::camDir.dot( P - persPt::camPos );// distance in front of camera
    if( U < 0.0f )
    {
        doDrawShadow = false;
        return;
    }

    (*pShadowVA)[0].position = persPt::get_xyw( Pg );
    // all but the last
    for( unsigned int n = 0; n < xfVec.size(); ++n )
    {
        P = pos + xfVec[n]*Xup + yfVec[n]*Yup;
        s = ( yGround - P.dot( Nsurf ) )/LtN;
        Pg = P + lightDir*s;
        if( persPt::camDir.dot( P - persPt::camPos ) < 0.0f )
        {
            doDrawShadow = false;
            return;
        }

        (*pShadowVA)[n+1].position = persPt::get_xyw( Pg );
    }

    // the last closes the fan
    (*pShadowVA)[ numPoints + 1 ].position = (*pShadowVA)[1].position;
}

void persFan::draw( sf::RenderTarget& RT ) const
{
    if( !( doDraw && inUse ) ) return;

    if( pSS )RT.draw( &( vtxVec[0] ), vtxVec.size(), sf::TriangleFan, &(pSS->txt) );
    else RT.draw( &( vtxVec[0] ), vtxVec.size(), sf::TriangleFan );
    // perimeter
 //   if( pPerim ) RT.draw( pPerim, numPoints + 1, sf::LinesStrip );
    if( !perimVec.empty() ) RT.draw( &( perimVec[0] ), perimVec.size(), sf::LinesStrip );
}

void persFan::constructAxes( vec3f Zu_p )// from given Zup and existing Yup
{
    // construct primed frame
    Zup = Zu_p;
    Zup /= Zup.mag();
    Xup = Yup.cross( Zup );
    Xup /= Xup.mag();
    Yup = Zup.cross( Xup );
}

void persFan::assignTexCoords( bool facingZ )
{
    if( !pSS ) return;

    size_t frIdx = 0;
    if( pSS->getFrCount(0) > 1 ) frIdx = ( facingZ ) ? 0 : 1;
 //   else return;// one side only

    sf::IntRect iRect = pSS->getFrRect( frIdx, 0 );
    sf::Vector2f txtCenter( iRect.left + 0.5f*iRect.width, iRect.top + 0.5f*iRect.height );
    float txtRadius = 0.5f*iRect.width;

 //   float dAngle = 2.0f*vec2f::PI/ numPoints;
    float Rratio = txtRadius/Rbound;

    vtxVec[0].texCoords = txtCenter;
    for( unsigned int j = 0; j < numPoints; ++j )
    {
     //   float angle = j*dAngle;
     //   vtxVec[j+1].texCoords.x = txtCenter.x;
     //   vtxVec[j+1].texCoords.x += facingZ ? txtRadius*cosf( angle ) : -txtRadius*cosf( angle );
     //   vtxVec[j+1].texCoords.y = txtCenter.y - txtRadius*sinf( angle );
        // based on xfVec, yfVec
        vtxVec[j+1].texCoords.x = txtCenter.x;// + Rratio*( xfVec[j] );
        vtxVec[j+1].texCoords.x += facingZ ? Rratio*( xfVec[j] ) : -Rratio*( xfVec[j] );
        vtxVec[j+1].texCoords.y = txtCenter.y - Rratio*( yfVec[j] );
    }

    vtxVec[ numPoints + 1 ].texCoords = vtxVec[1].texCoords;// closing fan

//    std::cout << "\n ATC: facingZ = " << facingZ;
}

void persFan::graphCircle()
{
    float dAngle = 2.0f*vec2f::PI/ numPoints;
    vtxVec[0].position = persPt::get_xyw( pos );
    for( unsigned int j = 0; j < numPoints; ++j )
    {
        float angle = j*dAngle;
        vec3f rp = pos + Rbound*( cosf( angle )*Xup + sinf( angle )*Yup );
        vtxVec[j+1].position = persPt::get_xyw( rp );
    }

    vtxVec[ numPoints + 1 ].position = vtxVec[1].position;// closing fan
}

void persFan::initCircle()// assign xfVec, yfVec at init
{
    if( xfVec.size() < numPoints || yfVec.size() < numPoints ) return;

    float dAngle = 2.0f*vec2f::PI/ numPoints;
    for( unsigned int j = 0; j < numPoints; ++j )
    {
        xfVec[j] = Rbound*cosf( j*dAngle );
        yfVec[j] = Rbound*sinf( j*dAngle );
    }
}

void persFan::initCircle( std::function<float(float)> R )// assign xfVec, yfVec at init. Given polar function R(angle)
{
    if( xfVec.size() < numPoints || yfVec.size() < numPoints ) return;

    float dAngle = 2.0f*vec2f::PI/ numPoints;
    Rbound = R( 0.0f );

    for( unsigned int j = 0; j < numPoints; ++j )
    {
        float r = R( j*dAngle );
        xfVec[j] = r*cosf( j*dAngle );
        yfVec[j] = r*sinf( j*dAngle );
        if( r > Rbound ) Rbound = r;
    }

    std::cout << "\n persFan::initCircle(R): Rbound = " << Rbound;
}

void persFan::updateCircle()// update Vertex positions
{
 //   float dAngle = 2.0f*vec2f::PI/ numPoints;
    // the fan
    vtxVec[0].position = persPt::get_xyw( pos );
    for( unsigned int j = 0; j < numPoints; ++j )
    {
        vtxVec[j+1].position = persPt::get_xyw( pos + xfVec[j]*Xup + yfVec[j]*Yup );
    //    if( pPerim ) pPerim[j].position = vtxVec[j+1].position;
    }
    vtxVec[ numPoints + 1 ].position = vtxVec[1].position;// closing fan

    // the perimeter
    if( !perimVec.empty() )
    {
        for( unsigned int j = 0; j < numPoints; ++j )
            perimVec[j].position = vtxVec[j+1].position;
        perimVec[ numPoints ].position = perimVec[0].position;// closing LinesStrip
    }
}

void persFan::pitch( float dAngle )
{
    Zup = Zup*cosf(dAngle) - Yup*sinf(dAngle);// pitch
    Zup /= Zup.mag();
    Yup = Zup.cross( Xup );
}

void persFan::yaw( float dAngle )
{
    Zup = Zup*cosf(dAngle) + Xup*sinf(dAngle);// yaw
    Zup /= Zup.mag();
    Xup = Yup.cross( Zup );
}

void persFan::roll( float dAngle )
{
    Yup = Yup*cosf(dAngle) + Xup*sinf(dAngle);// roll
    Yup /= Yup.mag();
    Xup = Yup.cross( Zup );
}

bool persFan::hit( vec3f posA, vec3f posB, vec3f& P, vec3f& vu )const// stationary only
{
    if( !doDraw ) return false;

    // does posB - posA cross the plane anywhere?
    float Ua = ( posA - pos ).dot( Zup );// projection a
    float Ub = ( posB - pos ).dot( Zup );// projection b
    if( Ub*Ua > 0.0f ) return false;// posA and posB are both on same side of plane
    vec3f rA = posA - pos;
    vec3f dPos = posB - posA;
    float s = -1.0f*rA.dot(Zup)/dPos.dot(Zup);
    P = rA + dPos*s;
    if( P.dot(P) > Rbound*Rbound ) return false;

    if( !isCircle )// find R
    {
        float angPol = atan2f( P.dot(Yup), P.dot(Xup) );// angle ccw from Xup
        if( angPol < 0.0f ) angPol += 2.0f*vec2f::PI;
        int idx = static_cast<int>( numPoints*angPol/(2.0f*vec2f::PI) );
        if( idx >= 0 && idx < static_cast<int>( numPoints ) && P.dot(P) > xfVec[idx]*xfVec[idx] + yfVec[idx]*yfVec[idx] )
            return false;
     //   std::cout << "\nNot a circle. idx = " << idx;
    }

    vu = dPos - 2.0f*( dPos.dot(Zup) )*Zup;
    vu /= vu.mag();
 //   vu -= 2.0f*( vu.dot(Zup) )*Zup;
    P += pos;
    return true;
}
