#ifndef EASEDMOTION_H
#define EASEDMOTION_H

#include<vector>
#include "../vec2f.h"

class easedMotion
{
    public:
    double tElap = 1.0, tFinal = 1.0;
    double u = 0.0;//  0 <= u <= 1
    double Y = 1.0;// eased u
    double X = 0.0, X0 = 0.0, Xf = 0.0f;

    // easing
    unsigned int basePow = 0;
    std::vector<double> cVec;// user supplied coefficients

    // start next if chaining
    easedMotion* pNext = nullptr;

    void init( double x0, double xf, double t_Final, unsigned int BasePow );
    void start();// start after init
    bool update( double dt );// true if tElap < tFinal after incrementation
    bool isAnimating()const { return tElap < tFinal; }
    double getAverage()const{ return delX/tFinal; }
    easedMotion(){}
    virtual ~easedMotion(){}

    protected:
    private:
        double delX = 0.0;
};

#endif // EASEDMOTION_H
