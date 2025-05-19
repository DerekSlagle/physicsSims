#ifndef LVL1_H_INCLUDED
#define LVL1_H_INCLUDED

#include "Level.h"
#include "../button_types/buttonValOnHit.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonList.h"
#include "../physicsSim.h"
#include "../functionPlot.h"

class lvlT : public Level
{
    public:

    float scale = 50.0f;// in pixels per meter
    double gravity = 9.807;// in meters per second^2
    float timeRate = 1.0f;// time passage rate = tSimulate/tReal
    float timeElap = 0.0f;
    sf::Vector2f simPos;

    sf::Text dtMsg, tElapMsg;
//    sf::Text simLabel;

    // controls
    controlSurface globalControls;
    multiSelector globalMS;// gravity, scale, timeRate
    buttonValOnHit globalStrip;
    buttonRect runButt, resetButt;
    buttonRect reportButt;// in use?
    buttonList simList;
    bool init_controls();

    physicsSim* pSim = nullptr;
    physicsSim sim_1;// pendulum
    physicsSim sim_2;// 2 masses
    physicsSim sim_3;// double pendulum
    physicsSim sim_4;// ball in pipe
    physicsSim sim_5;// pendulum with spring
    bool init_sims();

    // functions
    lvlT():Level() { std::cout << "Hello from lvl1 ctor\n"; }

    virtual bool init();

    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup(){}
    virtual ~lvlT() { cleanup(); }
};

#endif // LVL1_H_INCLUDED
