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
const double r_bohr = 5.29177e-11;// m

class LvlQM : public Level
{
    public:
    sf::Vertex wellVtxArray[6];
    void makeWell( const functionPlot& rWave_fp );

    functionPlot plot_1, plot_2;// energy states for finite square well
    double z0 = 1.0;
    std::vector<double> zVec;// roots found where plot_1 and plot_2 intersect
    void makePlot_1();
    void makePlot_2();

    functionPlot plot_3;// wavefunction for finite square well
    float V0, a, k, m;
    void makePlot_3( size_t rootIdx = 0 );

//    float fValue_1 = 0, fValue_2 = 0;// for plot_1.pValue to point to

    double subInts = 50.0;
    std::function<double(double)> pFunc_1 = nullptr, pFunc_1_even = nullptr, pFunc_1_odd = nullptr;// generates values for plot_1
    std::function<double(double)> pFunc_2 = nullptr, pFunc_2_even = nullptr, pFunc_2_odd = nullptr;// generates values for plot_2
 //   std::function<double(double)> pFuncWF_even = nullptr, pFuncWF_odd = nullptr, pFuncWF = nullptr;// generates values for plot_2
    // returns iterCount
    unsigned int rootFindIterLimit = 10;
    unsigned int findPlotIntersect( double x0, double dxMin, double& xInt, double& yInt, unsigned int iterLimit = 10 );

    // controls
    buttonList plotList;
    controlSurface plot12Control;
    buttonValOnHit z0Strip;

    controlSurface plot3Control;
    multiSelector msControl;
    buttonValOnHit msStrip;
    intSelector rootSelector;
    radioButton odd_evenSelector;
    buttonRect rootsButt;
    buttonRect addRootButt;

    // functions
    bool init_plots();
    bool init_controls();

    LvlQM():Level() { std::cout << "Hello from LvlQM ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup(){}
    virtual ~LvlQM() { cleanup(); }
};

#endif // LVLQM_H
