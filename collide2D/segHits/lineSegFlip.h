#ifndef LINESEGFLIP_H_INCLUDED
#define LINESEGFLIP_H_INCLUDED

#include "lineSegRotate.h"

class lineSegFlip : public lineSegRotate
{
    public:
    vec2d Lup, Ldn;
    float rotVelFlip;//
    int state;// -1=going down, 0=stopped, +1=going up
    bool isUp;

    lineSegFlip(): lineSegRotate() {}
    lineSegFlip( std::istream& fin );

    virtual ~lineSegFlip() {}
    virtual void init( std::istream& fin );
    void init( vec2d pt1, vec2d pt2, float Angle, float RotVel, sf::Color clr );

    virtual void update( float dt );
    virtual void to_file( std::ofstream& fout )const;

    bool is_up()const;
    void init_up();// trigger actions
    void init_dn();
    void set_up();// set state
    void set_dn();

 /*   virtual state_ab* newState();
    void setState( const float* pf );
    void getState( float* pf )const;    */
};

#endif // LINESEGFLIP_H_INCLUDED
