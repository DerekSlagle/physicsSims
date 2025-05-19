#ifndef TERRAINGRID_H
#define TERRAINGRID_H


#include "persPt.h"

// A grid square to world coordinates always at camPos
// covering visible scene out to Ag in front
class terrainGrid
{
    public:
    bool inUse = true;
    float Ag = 1000.0f;// distance to far side of grid from camera
    float dXg = 100.0f, dZg = 100.0f;// grid cell size
    vec3f origin;// for map y(x,z)
    sf::Color gridColor;
    std::vector<sf::VertexArray> xVAvec;// LinesStrips in x direction
    std::vector<sf::VertexArray> zVAvec;// LinesStrips in z direction
    std::vector<sf::Vertex> edgeStrip;
    vec3f h1, h2, PgLt, PgRt, PgA;
    std::function<float(float,float)> Yg = nullptr;

    void init( std::istream& is, std::function<float(float,float)> Yground = nullptr );
    void update();
    void draw( sf::RenderTarget& RT ) const;

    terrainGrid(){}
    ~terrainGrid(){}

    protected:
    private:
        float Bg = 1000.0f;// distance perpendicular to camDir at Ag: Bg = Ag*Ww/(@*Z0)
        float maxDim = 2000.0f;// the greater of Ag and 2*Bg
};

#endif // TERRAINGRID_H
