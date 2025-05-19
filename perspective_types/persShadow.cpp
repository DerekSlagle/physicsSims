#include "persShadow.h"

bool persShadow::shadowsOn = true;// merely assign to toggle drawing of shadows

void persShadow::init( persQuad& rPQ, vec3f& s_Hat, sf::Color color )
{
    sHat = s_Hat;
    for( unsigned int j = 0; j < 4; ++j ) shade[j].color = color;
    setPQ( rPQ );
}

void persShadow::setPQ( persQuad& rPQ )
{
    pPQ = &rPQ;
    Rbound = rPQ.Rbound;
    pos = rPQ.pos;
    isMoving = rPQ.isMoving;

    float SdotY = sHat.dot( persPt::yHat );

   for( unsigned int j = 0; j < 4; ++j )
    {
        vec3f B = pPQ->pt[j];// initial state
        Pgrd[j] = B -( B.dot( persPt::yHat )/SdotY )*sHat;
        shade[j].position = persPt::get_xyw( Pgrd[j] );
    }
}

void persShadow::update( float dt )
{
    if( !pPQ ) return;

    pPQ->update(dt);
    doDraw = pPQ->doDraw;// this is critical
    if( pPQ->isMoving ) pos = pPQ->pos;

    if( !persShadow::shadowsOn ) return;
//    if( SdotY > 0.0f ) { std::cout << "\n SdotY > 0"; return; }// light is shining upward = no shadow as no upper surface?

    // assign the vtx
    float SdotY = sHat.dot( persPt::yHat );
    for( unsigned int j = 0; j < 4; ++j )
    {
        vec3f B = pPQ->pt[j];
        Pgrd[j] = B - ( B.dot( persPt::yHat )/SdotY )*sHat;
        shade[j].position = persPt::get_xyw( Pgrd[j] );
    }
}

void persShadow::draw( sf::RenderTarget& RT ) const
{
    if( !pPQ ) return;
    if( !pPQ->doDraw ) return;

    //if( sHat.dot( persPt::yHat ) < 0.0f )
    if( persShadow::shadowsOn ) RT.draw( shade, 4, sf::Quads );
    pPQ->draw(RT);
}

// shadowAdapter
void shadowAdapter::init( persBox_quad& rPBQ, vec3f& s_Hat, sf::Color color )
{
    shPair[0].sHat = shPair[1].sHat = s_Hat;
    for( unsigned int j = 0; j < 4; ++j )
    {
        shPair[0].shade[j].color = color;
        shPair[1].shade[j].color = color;
    }
    setPBQ( rPBQ );
}
// change PBQ
void shadowAdapter::setPBQ( persBox_quad& rPBQ )
{
    pPBQ = &rPBQ;
    Rbound = rPBQ.Rbound;
    pos = rPBQ.pos;
    isMoving = rPBQ.isMoving;
    setPQ_pair();
}

// adjust to change in lighting. shPair may bind to other PQ. Called in update( vec3f, float )
void shadowAdapter::setPQ_pair()
{
    if( !pPBQ ) return;
    vec3f s_Hat = shPair[0].sHat;
    unsigned int i = 0;
    if( pPBQ->frontQ.Nu.dot( s_Hat ) > 0.0f ) { shPair[i].setPQ( pPBQ->frontQ ); ++i; }
    if( pPBQ->backQ.Nu.dot( s_Hat ) > 0.0f )  { shPair[i].setPQ( pPBQ->backQ ); ++i; }
    if( i < 2 && pPBQ->leftQ.Nu.dot( s_Hat ) > 0.0f ) { shPair[i].setPQ( pPBQ->leftQ ); ++i; }
    if( i < 2 && pPBQ->rightQ.Nu.dot( s_Hat ) > 0.0f ) { shPair[i].setPQ( pPBQ->rightQ ); ++i; }
    if( i < 2 && pPBQ->topQ.Nu.dot( s_Hat ) > 0.0f ) { shPair[i].setPQ( pPBQ->topQ ); ++i; }
    if( i < 2 && pPBQ->bottomQ.Nu.dot( s_Hat ) > 0.0f ) { shPair[i].setPQ( pPBQ->bottomQ ); ++i; }
}

void shadowAdapter::update( vec3f s_Hat, float dt )
{
    shPair[0].sHat = shPair[1].sHat = s_Hat;
    setPQ_pair();
    update(dt);
}

void shadowAdapter::update( float dt )
{
    if( !pPBQ ) return;
    if( pPBQ->isMoving ) pos = pPBQ->pos;

    pPBQ->update(dt);
    doDraw = pPBQ->doDraw;
    if( persShadow::shadowsOn )
    {
        shPair[0].update(dt);
        shPair[1].update(dt);
    }
}

void shadowAdapter::draw( sf::RenderTarget& RT ) const
{
    if( !pPBQ ) return;
    if( !pPBQ->doDraw ) return;

    if( persShadow::shadowsOn )
    {
        RT.draw( shPair[0].shade, 4, sf::Quads );
        RT.draw( shPair[1].shade, 4, sf::Quads );
    }

    pPBQ->draw(RT);
}
