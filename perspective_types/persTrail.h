#ifndef PERSTRAIL_H
#define PERSTRAIL_H

#include "persLine.h"
#include "../vec3d.h"

class persTrail
{
    public:
    persLine line[2];
    sf::Vertex sfc[4];
    bool doDraw = true;

    void draw( sf::RenderTarget& RT ) const;
    void init( vec3f StartPos, vec3f EndPos, float Width, sf::Color edgeColor, sf::Color surfaceColor );
    void init( std::istream& is );// calls above
    void update( float dt );

    persTrail(){}
    persTrail( vec3f StartPos, vec3f EndPos, float Width, sf::Color edgeColor, sf::Color surfaceColor ){ init( StartPos, EndPos, Width, edgeColor, surfaceColor ); }
    persTrail( std::istream& is ){ init(is); }
    ~persTrail(){}
};

#endif // PERSTRAIL_H
