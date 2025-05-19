#include "funcLeg.h"

void funcLeg::init( float X0, float Y0, float par0, float parF, char type, std::function<float(float)> pUv, std::function<float(float)> pUPv, size_t NumSegs )// type: c=cart, p=polar
{
    x0 = X0; y0 = Y0;
    numSegs = NumSegs;
    prev = next = nullptr;
    sVec.clear(); parVec.clear();
    sVec.reserve(numSegs+1); parVec.reserve(numSegs+1);
    if( type == 'c' )// cartesian
    {
        pYx = pUv;
        pYPx = pUPv;

    }
    else if( type == 'p' )// polar
    {
        pRa = pUv;
        pRPa = pUPv;
    }

    size_t N = 10*numSegs;
    float dPar = (parF - par0)/(float)N;
    float dPar_s = dPar > 0.0f ? dPar : -dPar;// positive
    float par = par0;
    float s = 0.0f;
    for( size_t i=0; i<N; ++i )
    {
        if( i%10 == 0 )
        {
            sVec.push_back(s);
            parVec.push_back(par);
    //        if( pRa ) std::cout << "\nang= " << par << " r= " << pRa(par) << " s= " << s;
        }

        if( pYPx )
        {
            float yP = pYPx(par);
            s += sqrtf( 1.0f + yP*yP )*dPar_s;
        }
        else if( pRa && pRPa )
        {
            float rP = pRPa(par), r = pRa(par);
            s += sqrtf( r*r + rP*rP )*dPar_s;
        }
        par += dPar;
    }

    sVec.push_back(s);
    parVec.push_back(par);

//    if( pRa ) std::cout << "\nang= " << par << " r= " << pRa(par) << " s= " << s;
//    if( pRPa ) std::cout << "\nk = " << pRPa(par) << " s = " << s << '\n';
    len = s;
    std::cout << "\nlen = " << len;
}

void funcLeg::init( Leg& rLeg, float par0, float parF, char type, std::function<float(float)> pUv, std::function<float(float)> pUPv, size_t NumSegs )
{
    x0 = rLeg.x( rLeg.len );// initial
    y0 = rLeg.y( rLeg.len );
    if( type == 'c' )// cartesian
    {
        pYx = pUv;
        pYPx = pUPv;
        x0 -= par0;// correct so initial position matches prev
        y0 -= pYx(par0);

    }
    else if( type == 'p' )// polar
    {
        pRa = pUv;
        pRPa = pUPv;
        float r0 = pUv(par0);
        x0 -= r0*cosf(par0);// correct so initial position matches prev
        y0 -= r0*sinf(par0);
    }

    init( x0, y0, par0, parF, type, pUv, pUPv, NumSegs );

    // link in
    rLeg.next = this;
    prev = &rLeg;
}

float funcLeg::x( float s )const
{
    float X = 0.0f;
    if( pYx ) X = x0 + find_par(s);// par = x
    else if( pRa )
    {
        float ang = find_par(s);// angle is parameter
        X = x0 + pRa(ang)*cosf(ang);
    }
    return X;
}

float funcLeg::y( float s )const
{
    float Y = 0.0f;
    if( pYx ) Y = y0 + pYx( find_par(s) );// par = x
    else if( pRa )
    {
        float ang = find_par(s);// angle is parameter
        Y = y0 + pRa(ang)*sinf(ang);
    }
    return Y;
}

void funcLeg::set_xy( float s, float& rx, float& ry ) const
{
    if( pYx )
    {
        rx = find_par(s);
        ry = y0+ pYx(rx);
        rx += x0;
    }
    else if( pRa )
    {
        float ang = find_par(s);
        rx = x0 + pRa(ang)*cosf(ang);
        ry = y0 + pRa(ang)*sinf(ang);
    }
}

float funcLeg::find_par( float s )const
{
    if( s < 0.0f ) { s = 0.0f; return parVec[0]; }
    if( s > len ) { s = len; return parVec.back(); }

//    int idx = (int) ((s*sVec.size())/len);// staring index for linear search of sVec for s
    int idx = (int)( ( s*(sVec.size()-2) )/len );// staring index for linear search of sVec for s

    while( idx > 0 && sVec[idx] > s ) --idx;// seek element just < s. If idx high
    while( idx + 1 < (int)sVec.size() && sVec[idx+1] < s ) ++idx;// if idx low

    // bound check
    if( idx < 0 ) { std::cout <<"\nlow idx = " << idx; idx = 0; }
    if( idx > (int)sVec.size() - 2 ) { std::cout <<"\nhigh idx = " << idx; idx = sVec.size() - 2; }
    // sVec[idx] < s < sVec[idx+1]
    float f = ( s - sVec[idx] )/( sVec[idx+1] - sVec[idx] );
 //   std::cout << idx << ' ';
    return parVec[idx] + f*( parVec[idx+1] - parVec[idx] );
}
