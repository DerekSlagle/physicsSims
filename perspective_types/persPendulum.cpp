#include "persPendulum.h"

vec3d persPendulum::Xu = vec3d::to_vec3d( persPt::xHat );
vec3d persPendulum::Yu = vec3d::to_vec3d( persPt::zHat );
vec3d persPendulum::Zu = vec3d::to_vec3d( persPt::yHat );

//vec3d persPendulum::Xu = vec3d(1.0,0.0,0.0);
//vec3d persPendulum::Yu = vec3d(0.0,1.0,0.0);
//vec3d persPendulum::Zu = vec3d(0.0,0.0,1.0);

void persPendulum::init( std::istream& is )
{

    is >> L >> bob.mass >> GravY >> numLoops;
    unsigned int rd, gn, bu; is >> rd >> gn >> bu;
    is >> th >> th_1 >> phi >> phi_1;
    th_2 = phi_2 = 0.0;
    updateLz();
    updateEtot();

    sf::Color color(rd,gn,bu);
    rod[1].color = rod[0].color = color;
    Lvtx[0].color = sf::Color::White;
    Lvtx[1].color = sf::Color::Green;
    topQ.init( is, nullptr );// pTxt = nullptr
    bob.init( is );
    topQ.facingCamera = false;
    Rbound = 10.0f;
    pos = topQ.pos;
    rod[0].position = Lvtx[0].position = topQ.get_xyw();
    rod[1].position = bob.get_xyw();
    update(0.0f);
}

void persPendulum::re_init()// following changes to dms
{
    updateLz();
    updateEtot();

    bool isMoving = bob.isMoving;
    bob.isMoving = true;
    update(0.0);
    bob.isMoving = isMoving;
}

double persPendulum::calcEtot()const
{
    return 0.5*( th_1*th_1 + phi_1*phi_1*sin(th)*sin(th) ) - ( GravY/L )*cos(th);
}

vec3d persPendulum::get_vel()const
{
    return vec3d( L*( th_1*get_thHat() + sin(th)*phi_1*get_phiHat() ) );
}

vec3d persPendulum::get_accel()const
{
    vec3d a = get_thHat()*( th_2*L );
    double sinTh = sin(th), cosTh = cos(th);
    a -= get_rHat()*( th_1*th_1 + th_1*phi_1*sinTh );
    a += get_phiHat()*( phi_2*sinTh + 2.0*th_1*phi_1*cosTh + phi_1*phi_1*sinTh*cosTh );
    return a;
}

void persPendulum::update_doDraw()
{
    topQ.update_doDraw();
    bob.update_doDraw();
    if( topQ.doDraw || bob.doDraw ) doDraw = true;
    else doDraw = false;
}

void persPendulum::updateRod()
{
 //   vec3d dSep = persPt.camDir.cross( rHat );
 //   double dSepMag = dSep.mag();
 //   if( dSepMag < 0.1 ){ dSep = thHat.cross( persPt::camDir ); dSepMag = dSep.mag(); }
 //   dSep /= dSepMag;
    rod[0].position = topQ.get_xyw();
    rod[1].position = bob.get_xyw();
    if( showL && topQ.doDraw )
    {
        Lvtx[0].position = topQ.get_xyw();
        // Ltot = RXV
    //    vec3d Ltot = L*get_rHat().cross( get_vel() );// Ltot = RxV
        double Lmag = Ltot.mag();
        Lvtx[1].position = Lvtx[0].position;
        if( Lmag > 1.0 )
        {
            Ltip = topQ.pos + ( Ltot*(120.0/Lmag) ).to_vec3f();
            Lvtx[1].position = persPt::get_xyw( Ltip );
        }
    }
}

// handles each of numLoops per frame with dT = dt/numLoops
void persPendulum::updateMotion( double& sinTh, double& cosTh, double& sinPhi, double& cosPhi, double dT )
{
    // update th and th_1
    th_2 = phi_1*phi_1*sinTh*cosTh - GravY*sinTh/L;
    double dTh = th_1*dT + 0.5*th_2*dT*dT;
    th += dTh;
    th_1 += th_2*dT;

    // update phi and phi_1
 //   phi_2 = -2.0*th_1*phi_1*cosTh/sinTh;
 //   double dPhi = phi_1*dT + 0.5*phi_2*dT*dT;
 //   phi += dPhi;
 //   phi_1 += phi_2*dT;

    phi_1 = Lz/( sinTh*sinTh );
    phi += phi_1*dT;

    // update sinTh, cosTh
    double temp = sinTh;
    temp += dTh*cosTh;// use updated value
    cosTh -= dTh*sinTh;// use updated value
    sinTh = temp;
    // update sinPhi, cosPhi
    temp = sinPhi;
    temp += dTh*cosPhi;// use updated value
    cosPhi -= dTh*sinPhi;// use updated value
    sinPhi = temp;
}

void persPendulum::update( float dt )
{
    update_doDraw();
    topQ.update(dt);

    if( bob.isMoving )
    {
        //  motion
        double dT = static_cast<double>(dt);
        double cosTh = cos(th), sinTh = sin(th), cosPhi = cos(phi), sinPhi = sin(phi);

        // the usual integration method
        if( numLoops > 1 )
            for( unsigned int j = 0; j < numLoops; ++j )updateMotion( sinTh, cosTh, sinPhi, cosPhi, dT/static_cast<double>( numLoops ) );
        else
        {
            updateMotion( sinTh, cosTh, sinPhi, cosPhi, dT );
            sinTh = sin(th);
            cosTh = cos(th);
            sinPhi = sin(phi);
            cosPhi = cos(phi);
        }

        // once per frame corrections to th_1 and phi_1 if observing conserved Lz and Etot
        if( useConservedLz )
        {
            // update phi_1
            double sinSq = sinTh*sinTh;
            if( sinSq > 1.0e-4 )
            {
                phi_1 = Lz/( sinSq );// based on Lz conservation
         //       std::cout << "Lz";
            }

        }

        if( useConservedEtot )
        {
            // update th_1
            double deter = 2.0*Etot + ( GravY/L )*cosTh - phi_1*Lz;// based on Etot conservation
            if( deter > 1.0e-4 )
            {
                double sign = th_1 > 0.0 ? 1.0 : -1.0;
                th_1 = sign*sqrt( deter );
         //       std::cout << "Etot";
            }
        }

        // bobs new velocity
        vec3d thHat = cosTh*( cosPhi*Xu + sinPhi*Yu ) + sinTh*Zu;// current thHat
        vec3d phiHat = cosPhi*Yu - sinPhi*Xu;// current phiHat
        Vel = L*( th_1*thHat + sinTh*phi_1*phiHat );
        bob.vel = Vel.to_vec3f();// bob.vel update
        vec3d rHat = sinTh*( cosPhi*Xu + sinPhi*Yu ) - cosTh*Zu;// updated rHat
        // and position
        vec3d Pos = L*rHat;
        Pos.x += static_cast<double>(topQ.pos.x);
        Pos.y += static_cast<double>(topQ.pos.y);
        Pos.z += static_cast<double>(topQ.pos.z);
        bob.setPosition( Pos.to_vec3f() );// bob update

        Ltot = L*rHat.cross( Vel );// Ltot = RxV
        // cycle value of phi
        const double twoPI = 2.0*3.1415927;
        if( phi_1 > 0.0 && phi > twoPI ) phi -= twoPI;
        else if( phi_1 < 0.0 && phi < -twoPI ) phi += twoPI;
    }
    else bob.update(dt);

    updateRod();
}

// This is working well, but is not accurate
bool persPendulum::hit( persBall& PB, float cR, float dt )
{
    if( bob.isMoving )
    {
        if( bob.hitFree( PB, cR, dt ) )
        {
            // find new values of th_1 and phi_1
            vec3d thHat = get_thHat();
            vec3d V;// bobs velocity after the hit
            V.x = static_cast<double>( bob.vel.x );
            V.y = static_cast<double>( bob.vel.y );
            V.z = static_cast<double>( bob.vel.z );

            // subtraction of (V.rHat)*rHat
            vec3d rHat = get_rHat();
            vec3d Vnew = V - ( V.dot(rHat) )*rHat;// bob cannot be moving along the rod
        //    bob.vel = Vel.to_vec3f();
            V = Vnew;// ?

            // update th_1 and phi_1
            th_1 = V.dot(thHat)/L;
            vec3d phiHat = get_phiHat();
            phi_1 = V.dot(phiHat)/( L*sin(th) );

            // update Vel and bob.vel ?
            Vel = get_vel();// based on updated th_1 and phi_1
            bob.vel = Vel.to_vec3f();
            // update Lz and Etot
            updateLz();
            updateEtot();

            return true;
        }

        return false;
        // done
    }
    else
    {
        // no change to state of bob occurs here. treated in frame where bob IS fixed (vRel is used)
        vec3f P, vu;
        if( !bob.hitFixed( PB.pos - PB.vel*dt, PB.Rbound, PB.pos, P, vu ) ) return false;
        PB.setPosition( P + 5.0f*vu );// a bit in front
        PB.vel = cR*PB.vel.mag()*vu;// bounce off
        return true;
    }

    return false;
}

/*
// TOTAL FAIL if bob.isMoving. Lz changes in wrong direction
// Works great when bob is in a fixed position
bool persPendulum::hit( persBall& PB, float cR, float dt )
{
 //   bool moving = bob.isMoving;// temp
 //   bob.isMoving = false;// temp

    vec3f P = PB.pos - bob.pos, vu;
 //   vec3f vRel = PB.vel - bob.vel;
    float Rsum = Rbound + PB.Rbound;
    float Rmin = bob.Rbound < PB.Rbound ? bob.Rbound : PB.Rbound;

    if( bob.isMoving )
    {
        // find P in frame where *this is at rest
        vec3f Vf = PB.vel - bob.vel, t1 = Vf/Vf.mag();
        if( Vf.mag()*dt < Rmin/4.0f )// low speed case
        {
            P = PB.pos - bob.pos;
            if( P.dot( Vf ) > 0.0f ) return false;// shot is receding
            if( P.dot(P) > Rsum*Rsum ) return false;
        //    std::cout << "\n hitFree() low speed";
        }
        else// high
        {
            vec3f A = PB.pos - Vf*dt, B = PB.pos;
            vec3f rA = A - bob.pos, rB = B - bob.pos;
            vec3f tn = rB.cross( t1 ); tn /= tn.mag();
            vec3f t2 = t1.cross( tn );
            float h = rB.dot( t2 );//, hMax = Rbound + Ball.Rbound;
            if(  h > Rsum ) return false;
            float w = sqrtf( Rsum*Rsum - h*h );
            P = h*t2 - w*t1;
            if( t1.dot( P - rA ) < 0.0f ) return false;
            if( t1.dot( rB - P ) < 0.0f ) return false;
         //   std::cout << "\n hitFree() high speed";
        }

        // this method is approximate
        // find velocities in cm frame
        vec3f pu = P/P.mag();
        float M = bob.mass + PB.mass;
        vec3f Vcm = ( PB.mass*PB.vel + bob.mass*bob.vel )/M;
        bob.vel -= Vcm; PB.vel -= Vcm;// shift to cm frame
        bob.vel -= ( 1.0f + cR )*( bob.vel.dot( pu ) )*pu;
        PB.vel  -= ( 1.0f + cR )*( PB.vel.dot( pu )  )*pu;
        bob.vel += Vcm; PB.vel += Vcm;// shift back to lab frame
        // bob cannot have a component of momentum in the rHat direction in the lab frame
        // subtract mass*vel.r = mass*(vel.rHat)*rHat from both balls.
        // MASSIVE PROBLEM: Lz changes in the wrong direction when bob is hit, so this all appears to be pure crap.
        // try not subtracting P.r = fail
        vec3f rHat = get_rHat().to_vec3f();// HEY what about the y,z axis swap??
        double temp = rHat.y; rHat.y = rHat.z; rHat.z = temp;// no help
        vec3f dP = bob.mass*( bob.vel.dot(rHat) )*rHat;// bobs P.r
     //   bob.vel -= dP/bob.mass;
     //   PB.vel -= bob.mass*dP/PB.mass;// this is a disaster. PB goes in a wildly wrong direction at extreme speed
     // it still changes in the wrong direction
        // find new values for th_1 and phi_1
        vec3f thHat = get_thHat().to_vec3f();// temp = thHat.y; thHat.y = thHat.z; thHat.z = temp;// no help
        th_1 = static_cast<double>( bob.vel.dot(thHat) )/L;
        vec3f phiHat = get_phiHat().to_vec3f();// temp = phiHat.y; phiHat.y = phiHat.z; phiHat.z = temp;// no help
        // try changing the sign of phi_1 = fail
     //   phi_1 = -static_cast<double>( bob.vel.dot(phiHat) )/( L*sin(th) );// nope! that's even worse
        phi_1 = static_cast<double>( bob.vel.dot(phiHat) )/( L*sin(th) );
        // update Lz and Etot
        updateLz();
        updateEtot();

        // assign positions
    //    vec3f r2 = topQ.pos + static_cast<float>( L )*rHat;
    //    bob.setPosition( r2 );
    //    PB.setPosition( r2 + Rsum*pu );// this causes a sudden massive jump in position

        PB.setPosition( bob.pos + Rsum*pu );// this looks ok
        return true;
    }
    else
    {
        // no change to state of bob occurs here. treated in frame where bob IS fixed (vRel is used)
        if( !bob.hitFixed( PB.pos - PB.vel*dt, PB.Rbound, PB.pos, P, vu ) ) return false;
        PB.setPosition( P + 5.0f*vu );// a bit in front
        PB.vel = cR*PB.vel.mag()*vu;// bounce off
    //    bob.isMoving = moving;// temp
        return true;
    }

 //   bob.isMoving = moving;// temp
    return false;
}

bool persPendulum::hit( persBall& PB, float cR, float dt )
{
    vec3f P, vu;
    vec3f vRel = PB.vel - bob.vel;

    bool moving = bob.isMoving;// temp
    bob.isMoving = false;// temp
    // in frame where bob IS fixed
    if( bob.hitFixed( PB.pos - vRel*dt, PB.Rbound, PB.pos, P, vu ) )// no change to state of bob
    {
        PB.setPosition( P + 5.0f*vu );// a bit in front
        PB.vel = cR*vRel.mag()*vu + bob.vel;// bounce off
        bob.isMoving = moving;// temp
        return true;
    }
    bob.isMoving = moving;// temp

    return false;
}
*/
void persPendulum::draw( sf::RenderTarget& RT ) const
{
    if( !doDraw ) return;
    if( topQ.doDraw && bob.doDraw ) RT.draw( rod, 2, sf::Lines );
    vec3f sep = topQ.pos - persPt::camPos;
//    bool drawTopQafterLvtx = ( sep.dot( Ltot.to_vec3f() ) > 0.0f );// Ltot is pointing away from the camera
        bool drawTopQafterLvtx = ( ( topQ.Nu.dot( sep ) )*( topQ.Nu.dot( Ltot.to_vec3f() ) ) > 0.0f );// Ltot and sep are facing topQ.Nu
//   if( showL && topQ.doDraw ) RT.draw( Lvtx, 2, sf::Lines );// draw before bob and before ( when facing camera) or after topQ

//    float
 //   bool drawTopQafterLvtx = ( ( topQ.Nu.dot( sep ) )*( topQ.Nu.dot( Ltot.to_vec3f() ) ) > 0.0f );

    if( topQ.getDistance() > bob.getDistance() )// bob is closer than topQ
    {
        if( showL && topQ.doDraw )
        {
            if( drawTopQafterLvtx ){ RT.draw( Lvtx, 2, sf::Lines ); topQ.draw(RT); }
            else { topQ.draw(RT); RT.draw( Lvtx, 2, sf::Lines ); }
        }
        else topQ.draw(RT);

        bob.draw(RT);
    }
    else// bob is further away than topQ
    {
        bob.draw(RT);

        if( showL && topQ.doDraw )
        {
            if( drawTopQafterLvtx ){ RT.draw( Lvtx, 2, sf::Lines ); topQ.draw(RT); }
            else { topQ.draw(RT); RT.draw( Lvtx, 2, sf::Lines ); }
        }
        else topQ.draw(RT);
     //   topQ.draw(RT);
    }
}

void persPendulum::setPosition( vec3f Pos )
{
    topQ.setPosition(Pos);
    vec3d R = get_rHat()*L;// rHat*L
    bob.setPosition( Pos + R.to_vec3f() );
    updateRod();
}
