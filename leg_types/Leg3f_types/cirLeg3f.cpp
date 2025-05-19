#include "cirLeg3f.h"

cirLeg3f::cirLeg3f(): Leg3f( vec3f(), 1.0f ), ctr( vec3f() ), T0(1.0f,0.0f,0.0f), Nu(0.0f,1.0f,0.0f)
{
    Tc = Nu.cross( T0 );
    R = 1.0f;
    angle = len/R;
}

// Dir = 'L' = left, 'R', 'U' and 'D' used to assign nu then call below
void cirLeg3f::init( vec3f Pos0, vec3f t0, char Dir, float r, float Angle )// 1st leg
{
    vec3f yHat(0.0f,1.0f,0.0f);
    vec3f nu;
    // NOTE: the turn is always to the right when nu is "up"
    if( Dir == 'L' )
    {
        vec3f tC = t0.cross( yHat );// points left
        nu = t0.cross( tC );// points down
    }
    else if( Dir == 'R' )
    {
        vec3f tC = yHat.cross( t0 );// points right
        nu = t0.cross( tC );// points up
    }
    else if( Dir == 'U' )// up
    {
        nu = t0.cross( yHat );// points left
    }
    else if( Dir == 'D' )// down
    {
        nu = yHat.cross( t0 );// points right
    }
    else return;

    init( Pos0, t0, nu, r, Angle );
}

void cirLeg3f::init( vec3f Pos0, vec3f t0, vec3f nu, float r, float Angle )// 1st leg
{
    // assure integrity of vector basis
    T0 = t0/t0.mag();
    Tc = nu.cross(t0);
    Tc /= Tc.mag();
    Nu = T0.cross(Tc);
    pos0 = Pos0;
    R = r;
    angle = Angle*PI/180.0f;
    len = R*angle;
    if( len < 0.0f ) len *= -1.0f;
    std::cout << " cirLeg: len = " << len << " R = " << R;
    ctr = pos0 + Tc*R;
    posF = getPos( len );

    prev = next = nullptr;
}

void cirLeg3f::init( Leg3f& rPrev, char Dir, float r, float Angle )// mid legs
{
    vec3f yHat(0.0f,1.0f,0.0f);
    vec3f t0 = rPrev.T( rPrev.len );
    vec3f nu;
    // NOTE: the turn is always to the right when nu is "up"
    if( Dir == 'L' )
    {
        vec3f tC = t0.cross( yHat );// points left
        nu = t0.cross( tC );// points down
    }
    else if( Dir == 'R' )
    {
        vec3f tC = yHat.cross( t0 );// points right
        nu = t0.cross( tC );// points up
    }
    else if( Dir == 'U' )// up
    {
        nu = t0.cross( yHat );// points left
    }
    else if( Dir == 'D' )// down
    {
        nu = yHat.cross( t0 );// points right
    }
    else return;

    init( rPrev, nu, r, Angle );
}

void cirLeg3f::init( Leg3f& rPrev, vec3f nu, float r, float Angle )// mid legs or terminal leg
{
    vec3f Pos0 = rPrev.getPos( rPrev.len );
    vec3f t0 = rPrev.T( rPrev.len );
    init( Pos0, t0, nu, r, Angle );

    prev = &rPrev;
    rPrev.next = this;
}

void cirLeg3f::init( Leg3f& rPrev, Leg3f& rNext, vec3f nu, float r, float Angle )// last (tie) leg
{
    vec3f Pos0 = rPrev.getPos( rPrev.len );
    vec3f t0 = rPrev.T( rPrev.len );
    init( Pos0, t0, nu, r, Angle );

    prev = &rPrev;
    rPrev.next = this;
    next = &rNext;
    rNext.prev = this;
}

void cirLeg3f::init( vec3f Pos0, Leg3f& rNext, vec3f t0, vec3f nu, float r, float Angle )// entry leg
{
    init( Pos0, t0, nu, r, Angle );
    next = &rNext;
    // do not link back
}

vec3f cirLeg3f::getPos( float s )const
{
    float Ang = angle*s/len;
    return ctr + R*( sinf(Ang)*T0 - cosf(Ang)*Tc );
}

vec3f cirLeg3f::rHat( float s )const
{
    float Ang = angle*s/len;
    return sinf(Ang)*T0 - cosf(Ang)*Tc;
}

vec3f cirLeg3f::T( float s )const
{
    float Ang = angle*s/len;
    return cosf(Ang)*T0 + sinf(Ang)*Tc;
}
