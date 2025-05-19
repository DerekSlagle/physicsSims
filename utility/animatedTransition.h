#ifndef ANIMATEDTRANSITION_H
#define ANIMATEDTRANSITION_H

#include<functional>
#include "../vec2f.h"

template<class T>
class animatedTransition
{
    public:
    static unsigned int numAnimating;
    float tElap = 1.0f, tFinal = 1.0f;
    T X0, delX;// delX = Xf - X0
    // easing
    std::function<float(float)> fy = nullptr;// Bezier motion, etc
    // update X from rate
    std::function<void(float)> fUpdateX = nullptr;

    // start next if chaining
    animatedTransition<T>* pNextAT = nullptr;

    void init( float tTrans, const T& rX0, const T& rXf );

    void start( float tTrans, const T& rX0, const T& rXf );// init and start
    void start();// start after init

    bool updateValue( T& X, float dt );// true if tElap < tFinal after incrementation
    bool updateRate( float& rate, float dt );// writes to rate
    bool updateRate( float dt );// uses fUpdateX

    bool isAnimating()const { return tElap < tFinal; }

    animatedTransition(){}
    virtual ~animatedTransition(){}

    protected:
    private:
};

template<class T>
unsigned int animatedTransition<T>::numAnimating = 0;

template<class T>
void animatedTransition<T>::init( float tTrans, const T& rX0, const T& rXf )
{
    X0 = rX0;
    delX = rXf - rX0;
    tFinal = tElap = tTrans;
}

template<class T>
void animatedTransition<T>::start( float tTrans, const T& rX0, const T& rXf )
{
    if( tElap < tFinal && numAnimating > 0 ) --numAnimating;// early restart

    X0 = rX0;
    delX = rXf - rX0;
    tFinal = tTrans;
    tElap = 0.0f;
    ++numAnimating;
}

template<class T>
void animatedTransition<T>::start()
{
    if( tElap < tFinal && numAnimating > 0 ) --numAnimating;// early restart

    tElap = 0.0f;
    ++numAnimating;
}

template<class T>
bool animatedTransition<T>::updateValue( T& X, float dt )// true if tElap < tFinal
{
    if( tElap < tFinal )
    {
        tElap += dt;
        float u = tElap/tFinal;
        if( u > 1.0f )
        {
            u = 1.0f;
            if( numAnimating > 0 ) --numAnimating;
        }

        if( fy ) u = fy(u);// mapping given
        X = X0 + delX*u;
        return tElap < tFinal;
    }

    return false;
}

template<class T>
bool animatedTransition<T>::updateRate( float& rate, float dt )// true if tElap < tFinal
{
    if( tElap < tFinal )
    {
        tElap += dt;
        float u = tElap/tFinal;
        if( u > 1.0f )
        {
            u = 1.0f;
            if( numAnimating > 0 ) --numAnimating;
            if( pNextAT ) pNextAT->start();
        }

        if( fy ) u = fy(u);// mapping given
        rate = delX*( u/tFinal );
        return tElap < tFinal;
    }

    return false;
}

template<class T>// call when fUpdateX is defined
bool animatedTransition<T>::updateRate( float dt )// true if tElap < tFinal
{
    if( tElap < tFinal )
    {
        tElap += dt;
        float u = tElap/tFinal;
        if( u > 1.0f )
        {
            u = 1.0f;
            if( numAnimating > 0 ) --numAnimating;
            if( pNextAT ) pNextAT->start();
        }

        if( fy ) u = fy(u);// mapping given
        float rate = delX*( u/tFinal );
        if( fUpdateX ) fUpdateX( rate*dt );
        return tElap < tFinal;
    }

    return false;
}

#endif // ANIMATEDTRANSITION_H
