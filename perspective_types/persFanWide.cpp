#include "persFanWide.h"

// just a circle or all numPoints given
void persFanWide::init( std::istream& is, spriteSheet* p_SS, spriteSheet* p_SSside )
{
    pSSside = p_SSside;
    unsigned int SetNum, FrIdx, txtSteps;
    is >> Width >> SetNum >> FrIdx >> txtSteps;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    sideColor = sf::Color(rd,gn,bu);
    // include perimeter?
    char ans = 'N'; is >> ans;
    if( ans == 'Y' )
    {
        is >> rd >> gn >> bu;
        perimColor = sf::Color(rd,gn,bu);
    }

    persFan::init( is, p_SS );
    sideVtxVec.resize( 2*numPoints + 2 );
    assignSideTexCoords( SetNum, FrIdx, txtSteps );
    if( ans == 'Y' ) initPerim( perimColor );
    updateCircle();// update Vertex positions
}

void persFanWide::assignSideTexCoords( unsigned int SetNum, unsigned int FrIdx, unsigned int txtSteps )
{
    for( sf::Vertex& V : sideVtxVec ) V.color = sideColor;
    // special mark the end
    sideVtxVec[ 2*numPoints + 1 ].color = sideVtxVec[ 2*numPoints ].color = sf::Color::Green;

    if( !pSSside ) return;
    if( SetNum >= pSSside->numSets() ) return;
    if( FrIdx >= pSSside->getFrCount( SetNum ) ) return;

    sf::IntRect frRect = pSSside->getFrRect( FrIdx, SetNum );
    // the starter line
    sideVtxVec[0].texCoords.x = sideVtxVec[1].texCoords.x = static_cast<float>( frRect.left );
    sideVtxVec[0].texCoords.y = static_cast<float>( frRect.top + frRect.height );
    sideVtxVec[1].texCoords.y = static_cast<float>( frRect.top );

    const float rectW = static_cast<float>( frRect.width );
    const float rectLt = static_cast<float>( frRect.left );

    unsigned int k = 1;// offset multiple k: 0 -> steps -> 0
    float dW = rectW/static_cast<float>( txtSteps );// offset
    bool fwd = true;// step direction

    for( unsigned int j = 1; j < numPoints + 1; ++j )
    {
        sideVtxVec[ 2*j ].texCoords.y = static_cast<float>( frRect.top + frRect.height );
        sideVtxVec[ 2*j + 1 ].texCoords.y = static_cast<float>( frRect.top );

        if( j%txtSteps == 0 )  fwd = !fwd;

        if( fwd ) k = j%txtSteps;
        else k = txtSteps - j%txtSteps;

        sideVtxVec[ 2*j ].texCoords.x = sideVtxVec[ 2*j + 1 ].texCoords.x = rectLt + k*dW;
    }
}

/*
void persFanWide::assignSideTexCoords( unsigned int SetNum, unsigned int FrIdx, unsigned int txtSteps )
{
    for( sf::Vertex& V : sideVtxVec ) V.color = sideColor;
    if( !pSSside ) return;
    if( SetNum >= pSSside->numSets() ) return;
    if( FrIdx >= pSSside->getFrCount( SetNum ) ) return;

    sf::IntRect frRect = pSSside->getFrRect( FrIdx, SetNum );
    // the starter line
    sideVtxVec[0].texCoords.x = sideVtxVec[1].texCoords.x = static_cast<float>( frRect.left );
    sideVtxVec[0].texCoords.y = static_cast<float>( frRect.top + frRect.height );
    sideVtxVec[1].texCoords.y = static_cast<float>( frRect.top );

    const float rectW = static_cast<float>( frRect.width );
    const float rectLt = static_cast<float>( frRect.left );

    unsigned int k = 1;// offset multiple k: 0 -> steps -> 0
    float dW = rectW/static_cast<float>( txtSteps );// offset
    bool fwd = true;

    for( unsigned int j = 1; j < numPoints + 1; ++j )
    {
        sideVtxVec[ 2*j ].texCoords.y = static_cast<float>( frRect.top + frRect.height );
        sideVtxVec[ 2*j + 1 ].texCoords.y = static_cast<float>( frRect.top );

        if( j%txtSteps == 0 )  fwd = !fwd;

        if( fwd ) k = j%txtSteps;
        else k = txtSteps - j%txtSteps;

        sideVtxVec[ 2*j ].texCoords.x = sideVtxVec[ 2*j + 1 ].texCoords.x = rectLt + k*dW;
    }
}
*/

void persFanWide::initPerim( sf::Color PerimColor )// fills perimVec. call after init()
{
    perimColor = PerimColor;

    perimVec.resize( 2*( numPoints + 1 ) );
    for( sf::Vertex& V : perimVec )    {
        V.color = perimColor;    }
}

void persFanWide::setPosition( vec3f Pos )
{
    pos = Pos;
    update_doDraw();
    if( doDraw ) updateCircle();
}

//void persFanWide::update( float dt )
//{ persFan::update(dt); }

void persFanWide::updateCircle()// update Vertex positions
{
    float hfW = 0.5f*Width;
    float U = Zup.dot( persPt::camPos - pos );
    float sf = ( U > 0.0f ) ? 1.0f : -1.0f;// offset factor for z direction. Face must be offset toward viewer
    bool foundLo = false, foundHi = false;// found iteration limits for draw

    vtxVec[0].position = persPt::get_xyw( pos + sf*hfW*Zup );

    vec3f Rspoke = xfVec[0]*Xup + yfVec[0]*Yup;// center to rim
    vec3f sep = persPt::camPos - ( pos + Rspoke );// rim to camera
    float sepRs = sep.dot( Rspoke );// draw when > 0.0f
    bool seekingHi = sepRs > 0.0f;// true if positive at start of iteration
    idxLo = 0; idxHi = numPoints;// iteration limits for draw
    doDrawSide = false;// until sepRs > 0.0f found

    for( unsigned int j = 0; j < numPoints; ++j )
    {
        Rspoke = xfVec[j]*Xup + yfVec[j]*Yup;
        sep = persPt::camPos - ( pos + Rspoke );
        sepRs = sep.dot( Rspoke );

        if( seekingHi && !foundHi && sepRs < 0.0f )
        {
            idxHi = j;
            foundHi = true;
            seekingHi = false;
        }
        else if( !seekingHi && !foundLo && sepRs > 0.0f )
        {
            idxLo = j;
            foundLo = true;
            seekingHi = !foundHi;
        }

        vtxVec[j+1].position = persPt::get_xyw( pos + Rspoke + sf*hfW*Zup );// face: offset in +Zu direction
        if( sepRs > 0.0f )// side vertex will be drawn
        {
            doDrawSide = true;
            sideVtxVec[ 2*j ].position = vtxVec[j+1].position;
            sideVtxVec[ 2*j + 1 ].position = persPt::get_xyw( pos + Rspoke - sf*hfW*Zup );// offset in -Zu direction
        }
    }

    vtxVec[ numPoints + 1 ].position = vtxVec[1].position;// closing fan
    sideVtxVec[ 2*numPoints ].position     = sideVtxVec[0].position;// closing
    sideVtxVec[ 2*numPoints + 1 ].position = sideVtxVec[1].position;// strip

    // the outlines
    if( !perimVec.empty() )
    {   // front
        for( unsigned int j = 0; j < numPoints; ++j )
            perimVec[ j ].position = vtxVec[j+1].position;

        if( doDrawSide )// back
            for( unsigned int j = 0; j < numPoints; ++j )
                perimVec[ numPoints + 1 + j ].position = sideVtxVec[ 2*j + 1 ].position;

        perimVec[ numPoints ].position = perimVec[0].position;// closing LinesStrip
        perimVec[ 2*numPoints + 1 ].position = sideVtxVec[1].position;// closing LinesStrip
    }
}

void persFanWide::updateShadow( vec3f lightDir, vec3f Nsurf, float yGround )
{
    if( !pShadowVA ) return;
    if( pShadowVA->getVertexCount() != 2 + numPoints ) return;
    doDrawShadow = true;// assumed until found false below

    float hfW = 0.5f*Width;
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

    float zf = lightDir.dot( Zup ) > 0.0f ?  1.0f: -1.0f ;
    (*pShadowVA)[0].position = persPt::get_xyw( Pg );

    // all but the last
    for( unsigned int n = 0; n < xfVec.size(); ++n )
    {
        vec3f Rspoke = xfVec[n]*Xup + yfVec[n]*Yup;
        float sf = lightDir.dot( Rspoke ) > 0.0f ?  -1.0f: 1.0f ;


        P = pos + Rspoke + zf*sf*hfW*Zup;// one line difference from base version
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

void persFanWide::draw( sf::RenderTarget& RT ) const
{
    if( !( doDraw && inUse ) ) return;

    if( doDrawSide )
    {
        // draw the side
        if( pSSside )
        {
            if( idxLo < idxHi )// one shot
                RT.draw( &( sideVtxVec[2*idxLo] ), 2*( idxHi - idxLo ), sf::TriangleStrip, &(pSSside->txt) );
            else// 2 parter
            {
                RT.draw( &( sideVtxVec[2*idxLo] ), 2*( numPoints + 1 - idxLo ), sf::TriangleStrip, &(pSSside->txt) );// Lo to end
                RT.draw( &( sideVtxVec[0] ), 2*idxHi, sf::TriangleStrip, &(pSSside->txt) );// begin to Hi
            }
        }
        else// with no texture
        {
            if( idxLo < idxHi )// one shot
                RT.draw( &( sideVtxVec[2*idxLo] ), 2*( idxHi - idxLo ), sf::TriangleStrip );
            else// 2 parter
            {
                RT.draw( &( sideVtxVec[2*idxLo] ), 2*( numPoints + 1 - idxLo ), sf::TriangleStrip );// Lo to end
                RT.draw( &( sideVtxVec[0] ), 2*idxHi, sf::TriangleStrip );// begin to Hi
            }
        }
        // the back side outline
        if( !perimVec.empty() )
        {
            if( idxLo < idxHi )// one shot
                RT.draw( &( perimVec[numPoints+1+idxLo] ), idxHi - idxLo, sf::LinesStrip );
            else// 2 parter
            {
                RT.draw( &( perimVec[numPoints+1+idxLo] ), numPoints + 1 - idxLo, sf::LinesStrip );// Lo to end
                RT.draw( &( perimVec[numPoints+1] ), idxHi, sf::LinesStrip );// begin to Hi
            }
        }
    }

    // face
    if( pSS )RT.draw( &( vtxVec[0] ), vtxVec.size(), sf::TriangleFan, &(pSS->txt) );
    else RT.draw( &( vtxVec[0] ), vtxVec.size(), sf::TriangleFan );

    // front side outline
    if( !perimVec.empty() ) RT.draw( &( perimVec[0] ), numPoints + 1, sf::LinesStrip );

}

// not in use
/*
void persFanWide::find_idxLoHi()// range of side elements facing the camera
{
    // existing working method
    // search for appearance of condition: sepRs to > 0 from < 0 (idxLo here) start drawing
    // and from sepRs to < 0 from > 0 (idxHi here) stop drawing
    vec3f Rspoke = xfVec[0]*Xup + yfVec[0]*Yup;// center to edge
    vec3f sep = persPt::camPos - ( pos + Rspoke );// rim to camera
    float sepRs = sep.dot( Rspoke );// find when this crosses 0.0f

    bool foundLo = false, foundHi = false;// iteration limits for draw
    bool seekingHi = sepRs > 0.0f;// true if positive at start of iteration
    idxLo = 0; idxHi = numPoints;

    for( unsigned int j = 0; j < numPoints; ++j )
    {
        Rspoke = xfVec[j]*Xup + yfVec[j]*Yup;
        sep = persPt::camPos - ( pos + Rspoke );
        sepRs = sep.dot( Rspoke );

        if( seekingHi && !foundHi && sepRs < 0.0f )
        {
            idxHi = j;
            foundHi = true;
            seekingHi = false;
        }
        else if( !seekingHi && !foundLo && sepRs > 0.0f )
        {
            idxLo = j;
            foundLo = true;
            seekingHi = !foundHi;
        }

        if( foundHi && foundLo ) return;// Done!
    }

    // coming soon: CALCULATE idxLo and idxHi
    // don't search blindly
}

// incomplete. not ready
void persFanWide::updateCircle_LoHi()
{
    float hfW = 0.5f*Width;
    float U = Zup.dot( persPt::camPos - pos );
    float sf = ( U > 0.0f ) ? 1.0f : -1.0f;// offset factor for z direction. Face must be offset toward viewer
    vec3f Rspoke = xfVec[0]*Xup + yfVec[0]*Yup;

    // update over full range for
    // face
    vtxVec[0].position = persPt::get_xyw( pos + sf*hfW*Zup );
    for( unsigned int j = 0; j < numPoints; ++j )
        vtxVec[j+1].position = persPt::get_xyw( pos + Rspoke + sf*hfW*Zup );// offset in +Zu direction

    vtxVec[ numPoints + 1 ].position = vtxVec[1].position;// closing fan

    // front side outline
    if( !perimVec.empty() )
    {
        for( unsigned int j = 0; j < numPoints; ++j )
            perimVec[ j ].position = vtxVec[j+1].position;

        perimVec[ numPoints ].position = perimVec[0].position;// closing LinesStrip
    }

    // The partial draws: side and rear outline
    // adapt the following code from draw() to update the corresponding vertex positions
    // the side
    if( idxLo < idxHi )// one shot
    {
   //     RT.draw( &( sideVtxVec[2*idxLo] ), 2*( idxHi - idxLo ), sf::TriangleStrip, &(pSSside->txt) );

        for( unsigned int j = idxLo; j < idxHi; ++j )
        {
            Rspoke = xfVec[j]*Xup + yfVec[j]*Yup;
            sideVtxVec[ 2*j ].position = vtxVec[j+1].position;
            sideVtxVec[ 2*j + 1 ].position = persPt::get_xyw( pos + Rspoke - sf*hfW*Zup );// offset in -Zu direction
        }
    }
    else// 2 parter
    {
     //   RT.draw( &( sideVtxVec[2*idxLo] ), 2*( numPoints + 1 - idxLo ), sf::TriangleStrip, &(pSSside->txt) );// Lo to end

        for( unsigned int j = idxLo; j < numPoints + 1; ++j )
        {
            Rspoke = xfVec[j]*Xup + yfVec[j]*Yup;
            sideVtxVec[ 2*j ].position = vtxVec[j+1].position;
            sideVtxVec[ 2*j + 1 ].position = persPt::get_xyw( pos + Rspoke - sf*hfW*Zup );
        }

   //     RT.draw( &( sideVtxVec[0] ), 2*idxHi, sf::TriangleStrip, &(pSSside->txt) );// begin to Hi

        for( unsigned int j = 0; j < idxHi; ++j )
        {
            Rspoke = xfVec[j]*Xup + yfVec[j]*Yup;
            sideVtxVec[ 2*j ].position = vtxVec[j+1].position;
            sideVtxVec[ 2*j + 1 ].position = persPt::get_xyw( pos + Rspoke - sf*hfW*Zup );
        }

        // close both ends
        sideVtxVec[ 2*numPoints ].position     = sideVtxVec[0].position;
        sideVtxVec[ 2*numPoints + 1 ].position = sideVtxVec[1].position;
    }


    // the back side outline
    if( !perimVec.empty() )
    {
        if( idxLo < idxHi )// one shot
        {
       //     RT.draw( &( perimVec[numPoints+1+idxLo] ), idxHi - idxLo, sf::LinesStrip );
            for( unsigned int j = idxLo; j < idxHi; ++j )
                perimVec[ numPoints + 1 + j ].position = sideVtxVec[ 2*j + 1 ].position;
        }
        else// 2 parter
        {
        //    RT.draw( &( perimVec[numPoints+1+idxLo] ), numPoints + 1 - idxLo, sf::LinesStrip );// Lo to end
            for( unsigned int j = idxLo; j < numPoints + 1; ++j )
                perimVec[ numPoints + 1 + j ].position = sideVtxVec[ 2*j + 1 ].position;

         //   RT.draw( &( perimVec[numPoints+1] ), idxHi, sf::LinesStrip );// begin to Hi
            for( unsigned int j = 0; j < idxHi; ++j )
                perimVec[ numPoints + 1 + j ].position = sideVtxVec[ 2*j + 1 ].position;
        }
    }

}
*/
