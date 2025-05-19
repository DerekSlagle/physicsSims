#include "persIsland.h"

const std::vector<persBox_quad>* persIsland::pBoxTemplateVec = nullptr;
const bool* persIsland::pAnyShots_inUse = nullptr;
std::vector<persBall>* persIsland::pShotVec = nullptr;
std::vector<persQuadAni>* persIsland::pAniVec = nullptr;
std::function<bool( persBall&, vec3f )> persIsland::pGetAniQuad = nullptr;

bool persIsland::init( const std::string& fName, std::vector<spriteSheet>& SSvec )
{
    std::ifstream fin( fName.c_str() );
    if( !fin ) { std::cout << "no island init data\n"; return false; }

    // picture
    unsigned int pictW, pictH; fin >> pictW >> pictH;
    picture.create( pictW, pictH );
    // island position
    fin >> pos.x >> pos.y >> pos.z;
    fin >> Rbound >> Rcam >> Rview >> R1;
    // pictQuad
 //   pictQuad.init( pos, pictW, pictH, vec3f(0.0f,0.0f,-1.0f), sf::Color::White, &( picture.getTexture() ) );
//    pictQuad.init( pos - Rbound*persPt::poscamDir, pictW, pictH, vec3f(0.0f,0.0f,-1.0f), sf::Color::White, nullptr );
 //   pictQuad.setTxtRect( sf::IntRect( 0, 0, pictW, pictH ), 'R', '0' );

 //   pictQuad.init( pos - Rbound*persPt::camDir, pictW, pictH, vec3f(0.0f,0.0f,-1.0f), sf::Color::White, &(SSvec[7].txt) );
 //   pictQuad.setTxtRect( SSvec[7].getFrRect( 0, 0 ), 'R', '1' );

    // persQuads
    size_t numQuads; fin >> numQuads;
    quadVec.reserve( numQuads );
    float W, H;// dimensions of each persQuad
    vec3f Pos, nu;
    size_t SSnum, SetNum, FrIdx;
    char oper, mode;
    for( size_t i = 0; i < numQuads; ++i )
    {
        fin >> Pos.x >> Pos.y >> Pos.z >> nu.x >> nu.y >> nu.z;
        fin >> W >> H >> SSnum >> SetNum >> FrIdx >> oper >> mode;
        Pos += pos;
        quadVec.push_back( persQuad( Pos, W, H, nu, sf::Color::White, &(SSvec[SSnum].txt) ) );
        quadVec.back().setTxtRect( SSvec[SSnum].getFrRect( FrIdx, SetNum ), oper, mode );
        if( nu.y < 0.8f ) quadVec.back().facingCamera = true;
    }

    if( !pBoxTemplateVec ){ std::cout << "\nBad pBoxTemplateVec"; return false; }

    // persBox_quad
    size_t numBoxes; fin >> numBoxes;
    quadBoxVec.reserve( numBoxes );
    pPBQ_targetVec.reserve( numBoxes );
    size_t tempNum;
    for( size_t i = 0; i < numBoxes; ++i )
    {
        fin >> tempNum >> Pos.x >> Pos.y >> Pos.z;
        if( tempNum >= pBoxTemplateVec->size() ) { std::cout << "\nBad template index"; return false; }
        Pos += pos;
        quadBoxVec.push_back( persBox_quad( (*pBoxTemplateVec)[ tempNum ] ) );
        quadBoxVec.back().setPosition( Pos );
        pPBQ_targetVec.push_back( &(quadBoxVec.back()) );
    }

    p_persPtVec.reserve( numQuads + numBoxes );
    for( persQuad& PQ : quadVec ) p_persPtVec.push_back( &PQ );
    for( persBox_quad& PQB : quadBoxVec ) p_persPtVec.push_back( &PQB );


    // initial image
/*    vec3f sep = pos - persPt::camPos;
    sep /= sep.mag();
    vec3f camPosSave = persPt::camPos;
    persPt::camPos = pos - Rbound*sep;
    updateIsland(0.0f);
 //   for( persPt* pt : p_persPtVec ) pt->update(0.0f);
    persPt::sortByDistance( p_persPtVec );
    picture.setActive();
    picture.clear( sf::Color(0,0,0,0) );
 //   drawIsland( picture );
    for( persPt* pt : p_persPtVec ) pt->draw( picture );
    picture.display();  */

    pictQuad.init( pos, pictW, pictH, vec3f(0.0f,0.0f,-1.0f), sf::Color::White, &( picture.getTexture() ) );
    pictQuad.setTxtRect( sf::IntRect( 0, 0, pictW, pictH ), 'R', '1' );

    vec3f sep = persPt::camPos - pos;
    takePicture( sep/sep.mag() );

 //   persPt::camPos = camPosSave;
    updateIsland(0.0f);
    persPt::sortByDistance( p_persPtVec );

    // diagnostic

    float Rind = 10.0f; fin >> Rind;
    float px, py; fin >> px >> py;
    alignInd.setRadius( Rind );// indicates a pictQuad alignment
    alignInd.setOrigin( Rind, Rind );
    alignInd.setPosition( px, py );
    alignInd.setFillColor( sf::Color::Blue );
    fin >> tLimitInd;
    tElapInd = tLimitInd;// count is from 0 to limit

    doDrawInd.setRadius( Rind );// indicates a pictQuad alignment
    doDrawInd.setOrigin( Rind, Rind );
    doDrawInd.setPosition( px + 4.0f*Rind, py );
    doDrawInd.setFillColor( doDraw ? sf::Color::Green : sf::Color::Red );

    return true;
}

void persIsland::update( float dt )
{
    bool doDrawLast = doDraw;
    update_doDraw();
    if( doDraw != doDrawLast ) doDrawInd.setFillColor( doDraw ? sf::Color::Green : sf::Color::Red );
    if( !doDraw ) return;

    vec3f sep = persPt::camPos - pos;
    distSq = sep.dot( sep );
    if( distSq > R1*R1 )// outside R1. update picture
    {
        if( isInsideR1 )
        {
            isInsideR1 = false;
            p_persPtVec.clear();
            for( persQuad& PQ : quadVec ) p_persPtVec.push_back( &PQ );
            for( persBox_quad& PQB : quadBoxVec ) p_persPtVec.push_back( &PQB );
            std::cout << "\ncrossed out";
        }

        vec3f sepU = sep/sep.mag();
        if( pictQuad.Nu.dot( sepU ) < viewAlignMin )
        {
           takePicture( sepU, dt );
            // diagnostic
            alignInd.setFillColor( sf::Color::White );
            tElapInd = 0.0f;// trigger
        }
        // move pictQuad each frame
        sep = persPt::camPos - pos;
//        sep *= Rbound/sep.mag();
        sep *= Rview/sep.mag();
        pictQuad.setPosition( pos + sep );
        pictQuad.update( dt );// position and scale
        // diagnostic
        if( tElapInd < tLimitInd && ( tElapInd+=dt ) >= tLimitInd )
            alignInd.setFillColor( sf::Color::Blue );
        // case: moving backwards from R1- to R1+
        if( alignInd.getFillColor() == sf::Color::Green ) alignInd.setFillColor( sf::Color::Blue );
    }
    else// inside R1. Draw island
    {
        if( !isInsideR1 )
        {
            isInsideR1 = true;
            for( persQuadAni& PQA : *pAniVec ) p_persPtVec.push_back( &PQA );
            for( persBall& PB : *pShotVec ) p_persPtVec.push_back( &PB );
            std::cout << "\ncrossed in";
        }
        updateIsland( dt );// for full draw
        alignInd.setFillColor( sf::Color::Green );
    }

    // hit testing
    if( pAnyShots_inUse && *pAnyShots_inUse && pShotVec && pGetAniQuad )
    {
        vec3f P, vu;
        // hit test against our targets
        for( persBall& PB : *pShotVec )
        {
            if( !PB.inUse ) continue;
            if( isInsideR1 )
            {
                for( persBox_quad* pPBQ : pPBQ_targetVec )
                {
                    vec3f P, vu;
                    if( pPBQ->hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
                    {
                        vec3f N = ( persPt::camPos - pPBQ->pos );
                        N /= N.mag();
                        pGetAniQuad( PB, P + 10.0f*N );
                        break;
                    }
                }
            }
            else if( pictQuad.hit( PB.pos - PB.vel*dt, PB.pos, P, vu ) )
                pGetAniQuad( PB, PB.pos );
        }
    }

}

void persIsland::takePicture( vec3f sepU, float dt )// sepU points from pos to camera
{
    vec3f camPosSave = persPt::camPos;
    vec3f camDirSave = persPt::camDir;
    // take picture from Rbound away facing toward pos
//    persPt::camPos = pos + Rbound*sepU;
    persPt::camPos = pos + Rcam*sepU;
//           persPt::camPos = pos - R1*sepU;
    persPt::camDir = -sepU;
    // level the view
    vec3f xuSave = persPt::xu, yuSave = persPt::yu;
    persPt::xu = persPt::yHat.cross( -sepU );
    persPt::xu /= persPt::xu.mag();
    persPt::yu = persPt::camDir.cross( persPt::xu );
    // position the canvas
    pictQuad.setNu( sepU );
    pictQuad.setPosition( persPt::camPos );
//            pictQuad.setPosition( pos );
    // take the picture
    updateIsland( dt );// all drawing positions assigned
    picture.clear( sf::Color(0,0,0,0) );// transparent background
    drawIsland( picture );// produce the picture
    picture.display();
    // restore camera position and orientation
    persPt::camPos = camPosSave;
    persPt::camDir = camDirSave;
    persPt::xu = xuSave;
    persPt::yu = yuSave;
}

void persIsland::updateIsland( float dt )// good from any camPos
{
    for( persBox_wf&  pb_wf : box_wfVec ) pb_wf.update( dt );
    for( persBox_quad& PBQ : quadBoxVec ) PBQ.update( dt );
    for( persQuad& PQ : quadVec ) PQ.update( dt );

    persPt::sortByDistance( p_persPtVec );
}

void persIsland::update_doDraw()
{
    doDraw = false;
//    if( persPt::camDir.dot( pictQuad.pos - persPt::camPos ) < -Rbound ) return;// don't draw when entirely behind the camera
    float U = persPt::camDir.dot( pos - persPt::camPos );// distance in front of camera
    if( U < -Rbound ) return;// don't draw when entirely behind the camera
//    sf::Vector2f winPos = persPt::get_xyw( pictQuad.pos );
    sf::Vector2f winPos = persPt::get_xyw( pos );
    float dim = U > 0.0f ? Rbound*persPt::Z0/U : 0.0f;// dimension = 0 if partially behind camera and at edge of window
    if( winPos.x + dim < 0.0f ) return;// left of window
    if( winPos.x - dim > 2.0f*persPt::X0 ) return;// right of window
    if( winPos.y + dim < 0.0f ) return;// above window
    if( winPos.y - dim > 2.0f*persPt::Yh ) return;// below window
    doDraw = true;
}

void persIsland::drawIsland( sf::RenderTarget& RT ) const// A NOT const version ?
{
    for( const persPt* pPt : p_persPtVec ) pPt->draw(RT);
}

void persIsland::draw( sf::RenderTarget& RT ) const
{
    if( doDraw )
    {
        if( distSq > R1*R1 ) pictQuad.draw( RT );
        else drawIsland( RT );
        RT.draw( alignInd );
    }
    // always drawn
    RT.draw( doDrawInd );
}
