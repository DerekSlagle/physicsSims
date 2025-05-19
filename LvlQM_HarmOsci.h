#ifndef LVLQM_HARMOSCI_H
#define LVLQM_HARMOSCI_H

#include "LvlQM.h"

class LvlQM_HarmOsci : public LvlQM
{
    public:

    double m = 1.0, k = 0.1;// mass and spring constant
    double Awf = 1.0, sf = 1.0;// amplitude of wave function, scale factor
    double Escale = 1.0;
    virtual void makePlot_v();
    virtual void makePlot_wf();

    // display superposition of states
    std::vector<double> superVec;// expansion coefficients read from file
    bool makePlot_super();// false if no file or bad data

    // messages

    // controls
    buttonRect superButt;

    // functions
    double HermitePoly( double x, unsigned int n );
    virtual double wf( double x, unsigned int n );// the wave function
    virtual double d_wf_dx( double x, unsigned int n );// 1st derivative wrt x
    virtual double d2_wf_dx2( double x, unsigned int n );// 2nd derivative wrt x
//    double wf_coeff( unsigned int n );// returns 1/sqrt( 2^n*n! )

    bool init_plots();
    bool init_controls();

    LvlQM_HarmOsci():LvlQM() { std::cout << "Hello from LvlQM_HarmOsci ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
//    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup(){}
    virtual ~LvlQM_HarmOsci() { cleanup(); }
};

#endif // LVLQM_HARMOSCI_H
