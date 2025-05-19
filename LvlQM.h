#ifndef LVLQM_H
#define LVLQM_H

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include<iostream>
#include<functional>

#include "Level.h"
#include "../button_types/buttonList.h"
#include "../button_types/controlSurface.h"
#include "../button_types/intSelector.h"
#include "../button_types/radioButton.h"
#include "../functionPlot.h"

const double PI = 3.14159265;
const double h_bar = 1.05475e-34;// J*s
const double r_Bohr = 5.29177e-11;// m
const double Me = 9.10938e-31;// mass of electron in Kg
const double Mp = 1.67262e-19;// mass of proton in Kg
const double Kb = 1.38065e-23;// Boltzmann constant in J/K
const double EVperJoule = 6.242e+18;

// abstract class
class LvlQM : public Level
{
    public:
    sf::Vertex energyLine[2];// drawn at energy level
    functionPlot plot_wf;// wave function
    functionPlot plot_v;// potential energy
    double subInts = 1000.0;// for graphing

    // for shaded area drawing
    std::vector<sf::Vertex> shadeVec;
    float shadeCenter = 0.0f, shadeWidth = 10.0f;// on graph scale
    void makeShade();
    bool initShadeControl( std::istream& is );
    void initSelectorControls( std::istream& is );

    // controls
    controlSurface plot3Control;
    multiSelector msControl;
    buttonValOnHit msStrip;
    radioButton wf_wfSqSelector;
    intSelector energySelector;
    controlSurface shadeSurf;// placed on plot3Control
    multiSelector shadeMs;
    buttonValOnHit shadeStrip;

    // messages
    sf::Text massMsg, massNumMsg;// in proton masses
    sf::Text energyMsg, energyNumMsg;// in electron volts
    sf::Text areaMsg, areaNumMsg;// area under wf^2
    sf::Text shadeAreaMsg, shadeAreaNumMsg;// shaded area
    sf::Text xSqExpectMsg, xSqExpectNumMsg;
    sf::Text pSqExpectMsg, pSqExpectNumMsg;
    sf::Text uncertMsg, uncertNumMsg;
    sf::Vertex msgSurface[4];// for drawing messages on

    sf::Text xCoordMsg;
    bool showCoord = false;


    // functions
    LvlQM():Level() { std::cout << "Hello from LvlQM ctor\n"; }
    void init( std::istream& is );

    virtual bool init() = 0;

    virtual void makePlot_v() = 0;
    virtual void makePlot_wf() = 0;

    virtual double wf( double x, unsigned int n ) = 0;// the wave function
    virtual double d_wf_dx( double x, unsigned int n ) = 0;// 1st derivative wrt x
    virtual double d2_wf_dx2( double x, unsigned int n ) = 0;// 2nd derivative wrt x

    virtual bool handleEvent( sf::Event& rEvent ) = 0;
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    virtual ~LvlQM() {};
};

#endif // LVLQM_H
