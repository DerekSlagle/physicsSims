#ifndef LVL_JUSTDRAW_H
#define LVL_JUSTDRAW_H

#include "Level.h"
#include "../button_types/controlSurface.h"
#include "../button_types/buttonValOnHit.h"
//#include "../button_types/slideBar.h"
//#include "../button_types/joyButton.h"
#include "vec2f.h"
#include "../spriteSheet.h"
#include "../utility/coordinateAxes.h"
#include "../utility/part3d.h"

class lvl_JustDraw : public Level
{
    public:
    std::vector<sf::Vertex> vtxVec;
    // from spriteSheet
    std::vector<spriteSheet> SSvec;

    vec3f Xu, Yu, Zu;// coordinate basis for object
    std::vector<part3d> theModel;

    // a flat part
    part3d partX;

    // for the part layout
    sf::CircleShape partCS;// marks start
    float RpartCS = 4.0f;
    coordinateAxes partAxes;// worldAxes?
    vec3f partOrigin;// for partAxes.pOrigin to point to

    // keys to animate
    bool LShiftKeyDown = false, RShiftKeyDown = false;
    bool YkeyDown = false, PkeyDown = false, RkeyDown = false;
    float rotSpeed = 1.0f, rotDir = 1.0f;// rotDir = -1.0f if shift key held

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    lvl_JustDraw():Level() { std::cout << "Hello from lvl_JustDraw ctor\n"; }
    void cleanup(){}
//    void cleanup(){}
    virtual ~lvl_JustDraw() { cleanup(); }

};

#endif // LVL_JUSTDRAW_H
