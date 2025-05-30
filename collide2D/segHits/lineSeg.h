#ifndef LINESEG_H_INCLUDED
#define LINESEG_H_INCLUDED

#include "segHit.h"

class lineSeg: public segHit
{
    public:
    vec2d L, N;
    float len;// magnitude of L
    sf::Vertex vtx[2];

    // funcs
    lineSeg(){}// don't use a default constructed lineSeg
    lineSeg( float x1, float y1, float x2, float y2, sf::Color clr = sf::Color::Green );
    void init( float x1, float y1, float x2, float y2, sf::Color clr = sf::Color::Green );
    void init( vec2d pt1, vec2d pt2, sf::Color clr = sf::Color::Green );
    lineSeg( vec2d pt1, vec2d pt2, sf::Color clr = sf::Color::Green ) { init( pt1, pt2, clr ); }
    lineSeg( std::istream& fin );
    virtual ~lineSeg();
    virtual void init( std::istream& fin );
    virtual void to_file( std::ofstream& fout )const;
    virtual void draw( sf::RenderTarget& rRW )const { rRW.draw(vtx, 2, sf::Lines); }
    virtual void setPosition( vec2d );

    virtual bool hit( mvHit& mh );
    virtual bool is_thruMe( vec2d pt1, vec2d pt2, vec2d& Pimp, float& fos )const;
    void deflect( mvHit& mh );

    vec2d getNearestPoint( vec2d pt );// returns vector from pt to nearest point on lineSeg
 //   bool is_onMe( const mvHit& mh, vec2d& Pimp, vec2d& Nh, float& pen )const;
 //   virtual vec2d getSurfaceNormal( const mvHit& mh )const { return L.get_LH_norm(); }
};

//sf::Vector2f& operator=( sf::Vector2f sfPos, vec2d pos ) { sfPos.x += pos.x; sfPos.y += pos.y; return sfPos; }


#endif // LINESEG_H_INCLUDED
