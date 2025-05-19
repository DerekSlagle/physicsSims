#ifndef COORDINATEAXES_H
#define COORDINATEAXES_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <fstream>
#include <functional>
#include "../vec2f.h"
//#include "../basis3f.h"

class coordinateAxes
{
    public:
    static const unsigned int Sz = 18;
    static std::function<sf::Vector2f(vec3f)> get_xyw;// mapping function given

    sf::Vertex axisVtx[Sz];
    vec3f axisPt[Sz];// relative position in basis
    sf::Text xLbl, yLbl, zLbl;
    // vec3f basis is updated elsewhere
    vec3f *pXu = nullptr, *pYu = nullptr, *pZu = nullptr;
    vec3f* pOrigin = nullptr;// to add in get_xyw()
    bool* pDoDraw = nullptr;
    // valid object?
    bool isValid = false;

    void update();// if get_xyw() given
    void draw( sf::RenderTarget& RT ) const;
    vec3f getPoint( unsigned int idx )const;// get the 3d position of each point
    void getLabelPos( vec3f& xLblPos, vec3f& yLblPos, vec3f& zLblPos );
    void setLabelColor( sf::Color color )
    {
        xLbl.setFillColor( color );
        yLbl.setFillColor( color );
        zLbl.setFillColor( color );
    }

    void init( std::istream& is, std::function<sf::Vector2f(vec3f)> get_xyw, const sf::Font* pFont );

    void init( sf::Color axesColor, float length, float offL, float offW, unsigned int charSz, std::function<sf::Vector2f(vec3f)> getXyw, const sf::Font* pFont );

    coordinateAxes();
    ~coordinateAxes();

    private:
};

#endif // COORDINATEAXES_H
