#include "persLine.h"

// persLine
void persLine::init( vec3f StartPos, vec3f EndPos, sf::Color color )
{
    startPos = StartPos;
    endPos = EndPos;
    vtx[0].color = vtx[1].color = color;
    pt[0] = startPos;
    pt[1] = endPos;
    dir = pt[1] - pt[0];
    dir /= dir.mag();
 //   update(0.0f);
    vtx[0].position = persPt::get_xyw( pt[0] );
    vtx[1].position = persPt::get_xyw( pt[1] );
}

void persLine::init( std::istream& is )// calls above
{
    vec3f StartPos, EndPos;
    is >> StartPos.x >> StartPos.y >> StartPos.z >> EndPos.x >> EndPos.y >> EndPos.z;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    sf::Color color(rd,gn,bu);
    persLine::init( StartPos, EndPos, color );
}

void persLine::update( float dt )
{
 //  float U0 = ( startPos - persPt::camPos ).dot( dir );
 //  float U1 = ( endPos - persPt::camPos ).dot( dir );
    float sf = dir.dot( persPt::camDir );
    float dDir = 1.0f;// increase or decrease dir component of pt positions
    if( sf < 0.0f ){ sf *= -1.0f; dDir = -1.0f; }
    if( sf < 0.1f ) sf = 0.1f;// NEW
   float U0 = ( startPos - persPt::camPos ).dot( persPt::camDir )/sf;
   float U1 = ( endPos - persPt::camPos ).dot( persPt::camDir )/sf;
   if( U0 < 0.0f && U1 < 0.0f )// line is behind camera
   {
 //      bool doDrawLast = doDraw;
       doDraw = false;
  //     if( doDrawLast ) std::cout << "\ndoDraw = false assigned";
       return;
   }

   doDraw = true;
    if( U0 < 0.0f ) pt[0] = startPos - dDir*( U0 + 10.0f )*dir;// pt[0] is behind camera
    else pt[0] = startPos;
    vtx[0].position = persPt::get_xyw( pt[0] );
    if( U1 < 0.0f ) pt[1] =   endPos - dDir*( U1 + 10.0f )*dir;// pt[1] is behind camera
    else pt[1] = endPos;
    vtx[1].position = persPt::get_xyw( pt[1] );
}

void persLine::draw( sf::RenderTarget& RT ) const
{
    if( doDraw )
        RT.draw( vtx, 2, sf::Lines );
}
