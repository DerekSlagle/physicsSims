#ifndef LVL_FOURIER_H
#define LVL_FOURIER_H

#include "Level.h"

#include "../button_types/buttonValOnHit.h"

class lvl_Fourier : public Level
{
    public:
    const double myPId = 3.14159265;
    std::vector<double> sinVec;// sine series expansion coefficients
    double L = 100.0;// half period for sine series
    std::vector<sf::Vertex> vtxVec;// plot of sine series
    std::vector<sf::Vertex> vtxVec_func;// plot of function
    sf::Vertex axes[4];
    float xOrigin = 0.0, yOrigin = 0.0;// drawing origin
    double dxVtx = 3.0;// spacing of sf::Vertex
    sf::Text coeffsMsg;// shows all coefficient values
    sf::Color plotColor, funcColor;
    double Amp = 100.0;// max value of function

    bool getPlotData();
    void makePlot( char inType );
    void updateCoeffsMsg();

    // animate transition between 2 given sets of coefficients
    std::vector<double> sinVecA, sinVecB;
    bool animating = false;
    float intFactor = 1.0f, tScale = 0.01f;
    void updatePlot();

    buttonRect rePlotButt;// reads plot data from file and makes plot
    buttonRect drawFuncButt;// draw function
    buttonRect drawAxesButt;// draw axes
    buttonRect drawDistA_Butt;// draw series A
    buttonRect drawDistB_Butt;// draw series B
    buttonRect animateButt;// animate between A and B
    buttonValOnHit tScaleStrip;
    bool init_controls();

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    lvl_Fourier(){}
    virtual ~lvl_Fourier(){}
};

#endif // LVL_FOURIER_H
