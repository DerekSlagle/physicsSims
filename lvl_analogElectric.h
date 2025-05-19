#ifndef LVL_ANALOGELECTRIC_H
#define LVL_ANALOGELECTRIC_H

#include "Level.h"
#include "oscilloscope.h"
#include "../button_types/controlSurface.h"
#include "../button_types/intSelector.h"
#include "../button_types/pushButton.h"

class analogDevice
{
public:
    sf::RectangleShape bodyRS;
    sf::Text label;
    double *pV_Ln = nullptr;// read
    double *pV_Ld = nullptr;// write
    analogDevice* pNext = nullptr;// call update() on the next AD
    virtual double getVoltageDrop()const = 0;// PV
    void init( std::istream& is );
    void draw( sf::RenderTarget& RT ) const { RT.draw( bodyRS ); RT.draw( label ); }
    virtual void update() = 0;// PV
};

// Vd = I*R
class resistor : public analogDevice// assigns *pV_Ld = *pV_Ln - I*R
{
    public :
    double R = 1.0f;
    double* pI = nullptr;
    virtual double getVoltageDrop()const { return *pI*R; }
    void init( std::istream& is, double& r_I );
    virtual void update();
};

// Vd = Q/C
class capacitor : public analogDevice// assigns *pV_Ld = *pV_Ln - Q/C
{
    public :
    double C = 1.0f;
    double* pQ = nullptr;
    virtual double getVoltageDrop()const { return *pQ/C; }
    void init( std::istream& is, double& r_Q );
    virtual void update();
};

// Vd = L*dIdt
class inductor : public analogDevice// assigns dIdt = ( *pV_Ln - *pV_Ld )/L
{
    public :
    double L = 1.0f;
    double* p_dIdt = nullptr;
    virtual double getVoltageDrop()const { return *p_dIdt*L; }
    void init( std::istream& is, double& r_dIdt );
    virtual void update();
};


class lvl_analogElectric : public Level
{
    public:
    const static double myPI;
    resistor R1;
    capacitor C1;
    inductor L1;

 //   functionPlot osciFP;
    oscilloscope osciR, osciC, osciL;
    oscilloscope osciI, osciVa;
    sf::Text Va_msg, I_msg, Vr_msg, Vc_msg, Vl_msg;// gives color key

  //  float tElap = 0.0f;
    size_t numReps = 5;
    double freq = 1.0f;
    double phaseAngle = 0.0f;
    double Q = 0.0f;// charge on C1
    double I = 0.0f;// current through all
    double dIdt = 0.0f;// inductor assigns this value
    double VaMax = 1.0f;
    double Va = 0.0f, Vb = 0.0f, Vc = 0.0f, Vd = 0.0f;// Va->R1->Vb->C1->Vc->L1->Vd
    sf::CircleShape VaCS, VbCS, VcCS, VdCS;

    // controls
 //   sf::RectangleShape controlRS;// to draw controls over
 //   buttonValOnHit VaMaxStrip;
 //   buttonValOnHit frequencyStrip;

    controlSurface paramsCS;
    multiSelector paramsMS;// R, L, C, VaMax, freq
    buttonValOnHit paramsStrip;
    unsigned int numPeriods = 1;
    intSelector numPeriodsSelector;
    pushButton pb_freqUp, pb_freqDown;// Both NOHC: tSweep value +/- while held. Return to assigned value on release
    pushButton pb_VsumButt;// to verify they sum to 0
    buttonRect sloMoButt;// one update per frame. numReps = 1
    bool init_controls();

    // functions
    lvl_analogElectric():Level() { std::cout << "Hello from lvl_analogElectric ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup(){}
    virtual ~lvl_analogElectric() { cleanup(); }
};

#endif // LVL_ANALOGELECTRIC_H
