#ifndef LVLQM_FSW_H
#define LVLQM_FSW_H

#include "LvlQM.h"

class LvlQM_FSW : public LvlQM
{
    public:
 //   sf::Vertex wellVtxArray[6];
 //   void makeWell( const functionPlot& rWave_fp );// fp needed for origin and scaling

    functionPlot plot_1, plot_2;// energy states for finite square well
    double z0 = 1.0;
    double Awf = 10.0;// amplitude of wave function
    double F = 1.0;// factor multiplying wf. F(a,k,z) assigned in makePlot_wf()
    std::vector<double> zVec;// roots found where plot_1 and plot_2 intersect
    void makePlot_1();
    void makePlot_2();

    double V0 = 100.0, a = 100.0, k = 1.0, m = 1.0;
    double L = 1.0;// L = z/a in makePlot_wf()
    float V0scale = 1.0e+14;// for graphing purpose
    virtual void makePlot_wf();
    virtual void makePlot_v();


    std::function<double(double)> pFunc_1 = nullptr, pFunc_1_even = nullptr, pFunc_1_odd = nullptr;// generates values for plot_1
    std::function<double(double)> pFunc_2 = nullptr, pFunc_2_even = nullptr, pFunc_2_odd = nullptr;// generates values for plot_2

    unsigned int rootFindIterLimit = 10;
    // returns iteration count
    unsigned int findPlotIntersect( double x0, double dxMin, double& xInt, double& yInt, unsigned int iterLimit = 10 );

    // controls
    buttonList plotList;
    controlSurface plot12Control;
    buttonValOnHit z0Strip;

    radioButton odd_evenSelector;
    buttonRect rootsButt;
    buttonRect addRootButt;

    buttonRect integrateButt;// Experiment. Integrate solution
    std::vector<sf::Vertex> plot_4Vec;

    // messages
    sf::Text aMsg, aNumMsg;// well width/2 in Bohr radii
    sf::Text V0Msg, V0NumMsg;// well depth in electron volts

    // functions
    bool init_plots();
    bool init_controls();

    virtual double wf( double x, unsigned int n );// the wave function
    virtual double d_wf_dx( double x, unsigned int n );// 1st derivative wrt x
    virtual double d2_wf_dx2( double x, unsigned int n );// 2nd derivative wrt x

    LvlQM_FSW():LvlQM() { std::cout << "Hello from LvlQM_FSW ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup(){}
    virtual ~LvlQM_FSW() { cleanup(); }
};

#endif // LVLQM_FSW_H
