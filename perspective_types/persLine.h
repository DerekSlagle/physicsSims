#ifndef PERSLINE_H
#define PERSLINE_H

#include "persPt.h"
#include "../vec3d.h"

class persLine
{
    public:
    vec3f startPos, endPos;
    vec3f pt[2];
    sf::Vertex vtx[2];
    vec3f dir;// line direction
    bool doDraw = true;
    void draw( sf::RenderTarget& RT ) const;
    void init( vec3f StartPos, vec3f EndPos, sf::Color color );
    void init( std::istream& is );// calls above
    void update( float dt );

    persLine(){}
    persLine( std::istream& is ){ init(is); }
    persLine( vec3f StartPos, vec3f EndPos, sf::Color color ){ init( StartPos, EndPos, color ); }
};

#endif // PERSLINE_H
