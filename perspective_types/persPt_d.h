#ifndef PERSPT_D_H
#define PERSPT_D_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <fstream>
#include <functional>
#include "../vec3d.h"
#include "../spriteSheet.h"

class persPt_d
{
    public:
    static float X0, Yh;// view point in window
    static double Z0;// for perspective scaling of x and y coordinates
    static vec3d camPos;// camera position in scene
    static vec3d camDir;// direction pointed
    static vec3d yu;    // unit vector "up" (camera orientation)
    static vec3d xu;    // unit vector "to right" : xu = camDir.cross( yu ) maintain in update()
    static const vec3d xHat, yHat, zHat;//(1.0f,0.0f,0.0f);
 //   const vec3f yHat(0.0f,1.0f,0.0f);
 //   const vec3f zHat(0.0f,0.0f,1.0f);
    static double angle;// yaw
    static double pitchAngle;
    static double rollAngle;

    static sf::Vector2f get_xyw( vec3d Pos );
    static double changeCamDir( double dAy, double dAp, double dAr );// returns compass heading
    static void calibrateCameraAxes();// maintenance
    static void sortByDistance( std::vector<persPt_d*>& pPtVec );

    vec3d pos;// position in scene
    double Rbound = 1.0;// boundary in front of camera. Closer = don't draw
    bool doDraw = true;// calculate in update() and use in zSort() and draw()
    bool isMoving = false;// no need to update position of object while off screen or behind camera

    void init( vec3d Pos ){ pos = Pos; }
    sf::Vector2f get_xyw()const { return persPt_d::get_xyw( pos ); }
    double getDistance()const{ return ( pos - persPt_d::camPos ).mag(); }
    virtual void setPosition( vec3d Pos ){ pos = Pos; }
    void setPosition( double X, double Y, double Z ) { setPosition( vec3d(X,Y,Z) ); }
    virtual void update( float dt ) = 0;
    virtual void draw( sf::RenderTarget& RT ) const = 0;
    virtual void update_doDraw();

    persPt_d(){}
    virtual ~persPt_d(){}
};

class persBall_d : public persPt_d
{
public:
    sf::CircleShape B;
    vec3d vel;// a moving ball
    float mass = 1.0f;
    bool inUse = true;

    void reset();
    void reset( vec3d Pos, vec3d Vel );

    void update_rw();
    bool hitFixed( vec3d posA, double r1, vec3d posB, vec3d& P, vec3d& vu )const;// *this is stationary
    bool hitFree( persBall_d& rB, double Cr, float dt );// both moving or 1 fixed (calls above). Collision is handled within function
    static void hitAll( std::vector<persBall_d>& PBvec, double Cr, float dt );// mutual collision handling
    void init( std::istream& is );
    void init( vec3d Pos, double R0, sf::Color color, vec3d Vel = vec3d() );
    virtual void setPosition( vec3d Pos );
    virtual void update( float dt );
    void update( vec3d grav, float dt );
    virtual void draw( sf::RenderTarget& RT ) const;
    persBall_d(){}
    persBall_d( std::istream& is ){ init(is); }
    virtual ~persBall_d(){}
};

#endif // PERSPT_D_H
