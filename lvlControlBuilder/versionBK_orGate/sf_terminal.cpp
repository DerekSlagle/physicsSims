#include "sf_terminal.h"

const sf::Color sf_terminal::termColorOff = sf::Color::Black;
const sf::Color sf_terminal::termColorOn = sf::Color::Yellow;
bool sf_terminal::doEcho = false;
bool sf_terminal::doDrawWires = false;

/*
void sf_terminal::init( sf_terminal* p_next, std::istream& is )
{
    pNext = p_next;
    is >> name;
    state = false;
    tmLabel.setFont( *button::pFont );
    tmLabel.setCharacterSize( 12 );
    tmLabel.setFillColor( sf::Color::Black );
    tmLabel.setString( name.c_str() );

    float R; is >> R;
    tmCS.setRadius(R);
    tmCS.setOrigin(R,R);
    float posX, posY; is >> posX >> posY;// position of terminal
    tmCS.setPosition( posX, posY );
    tmCS.setFillColor( sf_terminal::termColorOff );
    float offX, offY; is >> offX >> offY;
    tmLabel.setPosition( posX + offX, posY + offY );
}   */

void sf_terminal::init( sf_terminal* p_next, std::istream& is )
{
    std::string Name; is >> Name;
    float R, PosX, PosY, offX, offY;
    is >> R >> PosX >> PosY >> offX >> offY;
    init( p_next, Name, R, PosX, PosY, offX, offY );
}

void sf_terminal::init( sf_terminal* p_next, const std::string& Name, float R, float posX, float posY, float offX, float offY )
{
 //   pNext = p_next;
    name = Name;
    state = false;
    tmLabel.setFont( *button::pFont );
    tmLabel.setCharacterSize( 12 );
    tmLabel.setFillColor( sf::Color::Black );
    tmLabel.setString( name.c_str() );
    tmCS.setRadius(R);
    tmCS.setOrigin(R,R);
    tmCS.setPosition( posX, posY );
    set_pNext( p_next );
    tmCS.setFillColor( sf_terminal::termColorOff );
    tmLabel.setPosition( posX + offX, posY + offY );
    wireToNext[0].color = wireToNext[1].color = sf::Color::Black;
}

void sf_terminal::setState( bool st )
{
    if( state == st ) { if( doEcho ) std::cout << '\n' << name << " no state change. state = " << st; return; }
    state = st;
    if( doEcho ) std::cout << '\n' << name << ( st ? " energized" : " de energized" );
    tmCS.setFillColor( state ? sf_terminal::termColorOn : sf_terminal::termColorOff );
    if( pNext ) pNext->setState(st);// passing state to connected terminal
}

void sf_terminal::setPosition( sf::Vector2f pos )// call only after init()
{
    sf::Vector2f dPos = pos - tmCS.getPosition();
    tmCS.setPosition( pos );
    tmLabel.setPosition( dPos + tmLabel.getPosition() );
    wireToNext[0].position = wireToNext[1].position = getPosition();
    if( pNext ) wireToNext[1].position = pNext->getPosition();
}

void sf_terminal::draw( sf::RenderTarget& RT ) const
{
 //   if( doDrawWires && pNext ) RT.draw( wireToNext, 2, sf::Lines );
    RT.draw( tmCS );
    RT.draw( tmLabel );
    if( doDrawWires && pNext ) RT.draw( wireToNext, 2, sf::Lines );
}

void sf_terminal::drawWire( sf::RenderTarget& RT ) const// call if wires are to be drawn
{
    if( pNext ) RT.draw( wireToNext, 2, sf::Lines );
}

bool sf_terminal::pos_isOver( sf::Vector2f pos )const
{
    sf::Vector2f sep = pos - getPosition();
    float R = tmCS.getRadius();
    if( sep.x*sep.x + sep.y*sep.y < R*R ) return true;//{ std::cout << "\n sf_terminal::pos_isOver(): true"; return true; }
 //   std::cout << "\n sf_terminal::pos_isOver(): false";
    return false;
}

void sf_terminal::set_pNext( sf_terminal* p_Next )// also sets wireToNext positions
{
    pNext = p_Next;
    update_wirePosition();
}

void sf_terminal::update_wirePosition()
{
    wireToNext[0].position = wireToNext[1].position = getPosition();
    if( pNext ) wireToNext[1].position = pNext->getPosition();
}

// sf_loadTerminal
void sf_loadTerminal::init( std::function<void(bool)> pSetFunc, sf_terminal* p_next, std::istream& is )
{
    pSetState = pSetFunc;
    sf_terminal::init( p_next, is );
}

void sf_loadTerminal::setState( bool st )
{
    if( state == st ) { if( doEcho ) std::cout << '\n' << name << " no state change. state = " << st; return; }
    sf_terminal::setState(st);
    if( pSetState ) pSetState(st);// passing state to connected device
}

// sf_bus2
void sf_bus2::init( sf_terminal* p_termA, sf_terminal* p_termB, std::istream& is )
{
    pTermA = p_termA;
    pTermB = p_termB;
    sf_terminal::init( nullptr, is );
}

void sf_bus2::setState( bool st )
{
    sf_terminal::setState(st);// Hey! This is actually a bus3 object, but 1 pNext is unused
    if( pTermA ) pTermA->setState(st);
    if( pTermB ) pTermB->setState(st);
//    std::cout << "\n bus2::setState()";
}

// bus2out
sf::Color sf_bus2out::arrowColor = sf::Color::Blue;

void sf_bus2out::init( sf_terminal* p_termA, sf_terminal* p_termB, std::istream& is )
{
    sf_bus2::init( &tOutA, &tOutB, is );

    sf::Vector2f P0( getPosition() );// as defined only in base = sf_terminal
    float R, dx, dy, dxLbl, dyLbl; is >> R >> dx >> dy >> dxLbl >> dyLbl;
    std::cout << "\nR = " << R;
    tOutA.init( p_termA, "tA", R, P0.x + dx, P0.y + dy, dxLbl, dyLbl );
    is >> dx >> dy >> dxLbl >> dyLbl;
    tOutB.init( p_termA, "tB", R, P0.x + dx, P0.y + dy, dxLbl, dyLbl );
    // arrows
    sf::Vector2f PA = tOutA.getPosition(), PB = tOutB.getPosition();
    // setup vector basis ( r1, r2 ) for upper arrow
    sf::Vector2f r1 = PA - P0;// to be unit vector
    float rMag = sqrtf( r1.x*r1.x + r1.y*r1.y ) - tOutA.tmCS.getRadius();// tip to edge of tOut
    r1 /= rMag;// now unit length
    sf::Vector2f r2 = sf::Vector2f( -r1.y, r1.x );// LH normal to r1
    // setup vector basis ( s1, s2 ) for lower arrow
    sf::Vector2f s1 = PB - P0;// to be unit vector
    float sMag = sqrtf( s1.x*s1.x + s1.y*s1.y ) - tOutB.tmCS.getRadius();// tip to edge of tOut;
    s1 /= rMag;// now unit length
    sf::Vector2f s2 = sf::Vector2f( -s1.y, s1.x );// LH normal to s1

    // assign relative to P0. Will add P0 after
    float tipL, tipW; is >> tipL >> tipW;
    // upper
    arrows[0].position = r1*( rMag - tipL ) + r2*tipW;// end
    arrows[1].position = r1*rMag;// tip
    arrows[2].position = r1*( rMag - tipL ) - r2*tipW;// end
    arrows[3].position = r1*rMag;// tip
    // apex
    arrows[4].position = sf::Vector2f(0.0f,0.0f);// "tIn" position = P0
    // lower
    arrows[5].position = s1*sMag;// tip
    arrows[6].position = s1*( sMag - tipL ) + s2*tipW;// end
    arrows[7].position = s1*sMag;// tip
    arrows[8].position = s1*( sMag - tipL ) - s2*tipW;// end
    // add P0
    for( unsigned int n = 0; n < 9; ++n ){ arrows[n].position += P0; arrows[n].color = arrowColor; }
}

void sf_bus2out::draw( sf::RenderTarget& RT ) const
{
    // the 3 terminals
    sf_bus2::draw(RT);// tIn
    tOutA.draw(RT);
    tOutB.draw(RT);
    // the arrows
    RT.draw( arrows, 9, sf::LineStrip );
}

bool sf_bus2out::pos_isOver( sf::Vector2f pos )const
{
    sf::Vector2f P0 = getPosition(), PA = tOutA.getPosition(), PB = tOutB.getPosition();
    if( P0.x > PA.x ) { PA.x = P0.x; P0.x = PB.x; }// make PO the left

    if( pos.x < P0.x ) return false;// left of
    if( pos.x > PA.x ) return false;// right of
    if( pos.y < PA.y ) return false;// above
    if( pos.y > PB.y ) return false;// below
//    std::cout << "\n sf_bus2out::pos_isOver() true";
    return true;
}

void sf_bus2out::setPosition( sf::Vector2f pos )
{
    sf::Vector2f dPos = pos - getPosition();
    tmCS.setPosition( pos );
    tmLabel.setPosition( dPos + tmLabel.getPosition() );
    tOutA.setPosition( dPos + tOutA.getPosition() );
    tOutB.setPosition( dPos + tOutB.getPosition() );
    for( unsigned int n = 0; n < 9; ++n )
        arrows[n].position += dPos;
}

// busN
void sf_busN::init( const std::vector<sf_terminal*>& ptVec, std::istream& is )
{
    pTermVec = ptVec;
    sf_terminal::init( nullptr, is );
}

void sf_busN::setState( bool st )
{
    sf_terminal::setState(st);
    if( doEcho ) std::cout << "\nbusN: ";
    for( sf_terminal* pt : pTermVec )
        if( pt ){ pt->setState(st); if( doEcho ) std::cout << "\n*"; }

    if( doEcho ) std::cout << "\nEnd busN";
}

// sf_pushButton
void sf_pushButton::init( bool st, sf_terminal* Ld_pNext, std::istream& is )// st = true is NC
{
    is >> name;
    state = st;
    Ld.init( Ld_pNext, is );
    Ln.init( st ? &Ld : nullptr, is );
    line[0].position = Ln.tmCS.getPosition();
    line[1].position = Ld.tmCS.getPosition();
    line[0].color = line[1].color = sf::Color::Black;

    float posX, posY, W, H; is >> posX >> posY >> W >> H;// position of pbButt
    sf::Text label(name.c_str(), *button::pFont, 12);
    label.setFillColor( sf::Color::Black );
    label.setString( name.c_str() );
    pbButt.init( sf::Vector2f(posX, posY), sf::Vector2f(W,H), st ? 'C' : 'O', name, [this](){ setState(true); }, [this](){ setState(false); } );
 //   button::RegisteredButtVec.push_back( &pbButt );
}

void sf_pushButton::setState( bool st )
{
    if( state == st ) return;
    state = st;// open (0) or closed (1)
    if( st )
    {
        Ln.pNext = &Ld;
        if( sf_terminal::doEcho ) std::cout << "\n***  " << name << " closed  ***";
        if( Ld.state != Ln.state ) Ld.setState( Ln.state );
    }
    else
    {
        Ln.pNext = nullptr;
        if( sf_terminal::doEcho ) std::cout << "\n***  " << name << " opened  ***";
        if( Ld.state ) Ld.setState(false);
    }

 //   pbButt.setSel( state );
}

void sf_pushButton::draw( sf::RenderTarget& RT ) const
{
    if( state ) RT.draw( line, 2, sf::Lines );
    Ln.draw(RT);
    Ld.draw(RT);
}

void sf_pushButton::setPosition( sf::Vector2f Pos )// at Ln terminal
{
    sf::Vector2f dPos = Pos - getPosition();
 //   Ln.setPosition(Pos);
    Ld.setPosition( Ld.getPosition() + dPos );
    Ln.setPosition(Pos);
    pbButt.setPosition( pbButt.pos + dPos );
    line[0].position = Ln.tmCS.getPosition();
    line[1].position = Ld.tmCS.getPosition();
}

// sf_contact
void sf_contact::init( sf_terminal* pTermNo, sf_terminal* pTermNc, std::istream& is, sf_terminal* p_Next )
{
    sf_terminal::init( p_Next, is );// the "coil"
    sf::Vector2f coilPos = getPosition();

    bodyRS.setFillColor( sf::Color::Blue );
    bodyRS.setOutlineColor( sf::Color::Black );
    bodyRS.setOutlineThickness(3.0f);
    float ofstX, ofstY, W, H; is >> ofstX >> ofstY >> W  >> H;// offset from coil
    bodyRS.setSize( sf::Vector2f(W,H) );
    bodyRS.setPosition( coilPos.x + ofstX, coilPos.y + ofstY );

    com.init( com.state ? &no : &nc, is );// default state is open
    com.setPosition( bodyRS.getPosition() + com.getPosition() );
    no.init( pTermNo, is );// offset from bodyRS position is in file
    no.setPosition( bodyRS.getPosition() + no.getPosition() );
    nc.init( pTermNc, is );
    nc.setPosition( bodyRS.getPosition() + nc.getPosition() );

//    com.set_pNext( com.pNext );
    com.update_wirePosition();

//    line[0].position = com.getPosition();
 //   line[1].position = nc.getPosition();
 //   line[0].color = line[1].color = sf::Color::Black;

  //  pNext = p_Next;// usage unusual. "next" terminals are no and nc
    set_pNext( p_Next );// usage unusual. "next" terminals are no and nc
}

void sf_contact::init( sf_terminal* pTermNo, sf_terminal* pTermNc, sf::Vector2f Pos, float W, char facing, sf::Color clr, const std::string& name, sf_terminal* p_Next )
{
    float offX = 10.0f, offY = -10.0f, R = 4.0f;
 //   float offX = -28.0f, offY = 0.0f, R = 4.0f;
    sf_terminal::init( p_Next, name, 4.5, Pos.x, Pos.y, offX, offY );
    sf::Vector2f coilPos = getPosition();

    bodyRS.setFillColor( clr );
    bodyRS.setOutlineColor( sf::Color::Black );
    bodyRS.setOutlineThickness(2.0f);
    float H = W;
    offX = 0.0f; offY = -0.2f*H;// offset from coil
    bodyRS.setSize( sf::Vector2f(W,H) );
    bodyRS.setPosition( coilPos.x + offX, coilPos.y + offY );

    offX = 0.0f; offY = 0.7f*H;
    sf::Vector2f tPos = bodyRS.getPosition() + sf::Vector2f(offX,offY);
    float labelOffX = -28.0f, labelOffY = 0.0f;//0.7f*H;
    com.init( com.state ? &no : &nc, "com", R, tPos.x, tPos.y, labelOffX, labelOffY );// default state is open

    offX = W; offY = 0.3f*H;
    tPos = bodyRS.getPosition() + sf::Vector2f(offX,offY);
    labelOffX = 6.0f; labelOffY = -18.0f;//0.3f*H - 6.0f;
    no.init( pTermNo, "no", R, tPos.x, tPos.y, labelOffX, labelOffY );

    offX = W; offY = 0.7f*H;
    tPos = bodyRS.getPosition() + sf::Vector2f(offX,offY);
    labelOffX = 6.0f; labelOffY = 0.0f;//0.7f*H;
    nc.init( pTermNc, "nc", R, tPos.x, tPos.y, labelOffX, labelOffY );

 //   line[0].position = com.getPosition();
 //   line[1].position = nc.getPosition();
 //   line[0].color = line[1].color = sf::Color::Black;

    com.wireToNext[1].position = com.pNext ? com.pNext->getPosition() : com.wireToNext[0].position;

  //  pNext = p_Next;// usage unusual. "next" terminals are no and nc
    set_pNext( p_Next );// usage unusual. "next" terminals are no and nc
}

void sf_contact::setState( bool st )
{
    if( state == st ) return;
    state = st;
    if( doEcho ) std::cout << '\n' << name << ( st ? " closed" : " opened" );
    tmCS.setFillColor( state ? sf_terminal::termColorOn : sf_terminal::termColorOff );
 //   line[1].position = st ? no.getPosition() : nc.getPosition();
    if( com.pNext ) com.pNext->setState(false);// current terminal de energized
//    if( com.pNext && com.pNext->state ) com.pNext->setState(false);// current terminal de energized
//    com.pNext = st ? &no : &nc;// contact closes : opens
    com.set_pNext( st ? &no : &nc );// contact closes : opens
    com.pNext->setState( com.state );
//    if( com.pNext && ( com.pNext->state != com.state ) ) com.pNext->setState( com.state );
    if( pNext ) pNext->setState( st );
}

void sf_contact::draw( sf::RenderTarget& RT ) const
{
    RT.draw( bodyRS );
    sf_terminal::draw(RT);// the "coil" terminal
 //   RT.draw( line, 2, sf::Lines );
    com.draw(RT);
    if( !doDrawWires ) RT.draw( com.wireToNext, 2, sf::Lines );
    no.draw(RT);
    nc.draw(RT);
}

void sf_contact::setPosition( sf::Vector2f pos )
{
    sf::Vector2f dPos = pos - getPosition();
    sf_terminal::setPosition( pos );
    bodyRS.setPosition( dPos + bodyRS.getPosition() );
 //   tmLabel.setPosition( tmLabel.getPosition() + dPos );
 //   com.setPosition( com.getPosition() + dPos );
    no.setPosition( no.getPosition() + dPos );
    nc.setPosition( nc.getPosition() + dPos );
    com.setPosition( com.getPosition() + dPos );
 //   line[0].position += dPos;
 //   line[1].position += dPos;
}

bool sf_contact::pos_isOver( sf::Vector2f pos, sf_terminal*& rpTermOver )
{
    if( sf_terminal::pos_isOver( pos ) ) { rpTermOver = this; return true; }
    if( com.pos_isOver( pos ) ) { rpTermOver = &com; return true; }
    if( no.pos_isOver( pos ) ) { rpTermOver = &no; return true; }
    if( nc.pos_isOver( pos ) ) { rpTermOver = &nc; return true; }

    rpTermOver = nullptr;
    sf::FloatRect rect = bodyRS.getGlobalBounds();
    if( pos.x < rect.left ) return false;
    if( pos.x > rect.left + rect.width ) return false;
    if( pos.y < rect.top ) return false;
    if( pos.y > rect.top + rect.height ) return false;
    return true;
}

sf_contact& sf_contact::operator=( const sf_contact& C )
{
    // inherited members
    name = C.name;
    state = false;
    pNext = nullptr;
    tmCS = C.tmCS;
    tmLabel = C.tmLabel;
    //
    bodyRS = C.bodyRS;
    com = C.com;// except for below
//    com.pNext = state ? &no : &nc;// the only reason to define =
    com.set_pNext( state ? &no : &nc );// the only reason to define =
    no = C.no;
    nc = C.nc;
    no.pNext = nc.pNext = nullptr;
 //   line[0] = C.line[0];
 //   line[1] = C.line[1];
    return *this;
}

// sf_limitSwitch
void sf_limitSwitch::init( std::istream& is, sf_terminal* pTermNo, sf_terminal* pTermNc, sf_terminal* p_Next )
{
    sf_contact::init( pTermNo, pTermNc, is, p_Next );

    is >> Xopen >> Nu.x >> Nu.y;
    float mag = sqrtf( Nu.x*Nu.x + Nu.y*Nu.y );
 //   int dir = Nu.x;
    sf::Vector2f ofst; is >> ofst.x >> ofst.y;// to image of switch
 //   float H, W;
    is >> Wsw >> Hsw;// of limit switch part of image
    vtx[0].color = vtx[1].color = vtx[2].color = sf::Color::Black;
    vtx[2].position = ofst;
    setNu( Nu );

 //   vtx[0].position = vtx[1].position = vtx[2].position = ofst;
 //   vtx[0].position.x -= dir == 1 ? Wsw : -Wsw;
 //   vtx[1].position.x -= dir == 1 ? Wsw : -Wsw;
  //  vtx[0].position.y += 0.5f*Hsw;
  //  vtx[1].position.y -= 0.5f*Hsw;

    float R; is >> R;
    cs.setRadius(R);
    cs.setOrigin(R,R);
    cs.setPosition( vtx[2].position );
    cs.setFillColor( sf::Color::Blue );
    cs.setOutlineColor( sf::Color::Black );
    cs.setOutlineThickness( 2.0f );
}

void sf_limitSwitch::update( float x )
{
    if( x <= Xopen ) setState( true );
    else setState( false );
}

void sf_limitSwitch::update( sf::Vector2f pos )// must pass within Xopen to activate
{
    sf::Vector2f sep = pos - vtx[2].position;
    if( sep.x*sep.x + sep.y*sep.y > Xopen*Xopen )
    {
        if( state ) setState(false);
        return;// not there yet or a miss
    }

    float x = sep.x*Nu.x + sep.y*Nu.y;// projection of sep in Nu direction
    update(x);
}

/*
void sf_limitSwitch::update( float x )
{
    if( dir == 1 )
    {
        if( x <= Xopen ) setState( true );
        else setState( false );
    }
    else
    {
        if( x >= Xopen ) setState( true );
        else setState( false );
    }
}
*/

void sf_limitSwitch::setState( bool st )
{
    bool oldState = state;
    sf_contact::setState(st);// sb com.state not st as arg?.
    if( oldState != state )// state change
    {
    //    if( state ) vtx[2].position.x = 0.5f*( vtx[0].position.x + vtx[2].position.x );// gap to half
    //    else vtx[2].position.x += vtx[2].position.x - vtx[0].position.x;// gap doubles
        sf::Vector2f midPos = 0.5f*( vtx[0].position + vtx[1].position );
        if( state ) vtx[2].position = midPos + 0.5f*Wsw*Nu;// gap to half
        else vtx[2].position = midPos + Wsw*Nu;// gap to full
        cs.setPosition( vtx[2].position );
    }
}

void sf_limitSwitch::draw( sf::RenderTarget& RT ) const
{
 //   sf_contact::draw(RT);
    RT.draw( bodyRS );
//    sf_terminal::draw(RT);// the "coil" terminal
    com.draw(RT);
    if( !doDrawWires ) RT.draw( com.wireToNext, 2, sf::Lines );
    no.draw(RT);
    nc.draw(RT);

    RT.draw( vtx, 3, sf::LineStrip );
    RT.draw( cs );
}

void sf_limitSwitch::setPosition( sf::Vector2f pos )// call only after init()
{
    sf::Vector2f dPos = pos - getPosition();
    sf_contact::setPosition( pos );
    cs.setPosition( dPos + cs.getPosition() );
    for( unsigned int i = 0; i < 3; ++i ) vtx[i].position += dPos;
}

// set switch orientation
void sf_limitSwitch::setNu( sf::Vector2f n )
{
    Nu = n;
    sf::Vector2f Su( -n.y, n.x );
    if( n.x < 0.0f ) Su *= -1.0f;// vtx[1].y > vtx[1].y always upright
    vtx[0].position = vtx[2].position - Nu*Wsw + Su*0.5f*Hsw;
    vtx[1].position = vtx[0].position - Su*Hsw;
}

// for moving each part
void sf_limitSwitch::setContactPosition( sf::Vector2f pos )
{
    sf::Vector2f dPos = pos - getPosition();
    sf_contact::setPosition( pos );
}

void sf_limitSwitch::setSwitchPosition( sf::Vector2f pos )
{
    sf::Vector2f dPos = pos - cs.getPosition();
    cs.setPosition( pos );
    for( unsigned int i = 0; i < 3; ++i ) vtx[i].position += dPos;
}

/*
bool sf_limitSwitch::hitSwitch( sf::Vector2f P )const
{
    if( P.y < vtx[1].position.y ) return false;// above
    if( P.y > vtx[0].position.y ) return false;// below
    if( P.x < vtx[0].position.x ) return false;// to left
    if( P.x > vtx[2].position.x ) return false;// to right
    return true;
}
*/

bool sf_limitSwitch::hitSwitch( sf::Vector2f P )const
{
    sf::Vector2f P0 = vtx[2].position;
    sf::Vector2f S = P - P0;
    float SdotNu = S.x*Nu.x + S.y*Nu.y;

    if( SdotNu > cs.getRadius() ) return false;// right
    if( SdotNu < -Wsw ) return false;// left
    S -= SdotNu*Nu;// now = component perpendicular to Nu
    float Smag = sqrtf( S.x*S.x + S.y*S.y );
    if( Smag > 0.5f*Hsw ) return false;// above or below

    return true;
}

// sf_orGate
void sf_orGate::init( sf_terminal* Ld_pNext, std::istream& is )
{
//    Ld.init( Ld_pTerm, "gtLd" );
    is >> name;

    bodyRS.setFillColor( sf::Color::Magenta );
    bodyRS.setOutlineColor( sf::Color::Black );
    bodyRS.setOutlineThickness(3.0f);
    sf::Vector2f pos;
    float W, H; is >> pos.x >> pos.y >> W  >> H;// bodyRS
    bodyRS.setSize( sf::Vector2f(W,H) );
    bodyRS.setPosition( pos );

    label.setFont( *button::pFont );
    label.setCharacterSize(14);
    label.setFillColor( sf::Color::Black );
    label.setString( name.c_str() );
    sf::Vector2f ofst; is >> ofst.x >> ofst.y;
    label.setPosition( pos + ofst );

    tA.init( [this](bool st){ setState(); }, nullptr, is );// offset given from bodyRS
    tA.setPosition( pos + tA.getPosition() );
    tB.init( [this](bool st){ setState(); }, nullptr, is );
    tB.setPosition( pos + tB.getPosition() );
    tOut.init( Ld_pNext, is );
    tOut.setPosition( pos + tOut.getPosition() );
 //   state = tA.state || tB.state;
}

void sf_orGate::setState()// called by tA and tB
{
    bool st = tA.state || tB.state;
    tOut.setState( st );
    if( sf_terminal::doEcho ) std::cout << "\nGate.state = " << ( st ? "on" : "off" );
}

void sf_orGate::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - getPosition();// bodyRS = position
    bodyRS.setPosition( Pos );
    tOut.setPosition(  dPos + tOut.getPosition() );
    tA.setPosition( dPos + tA.getPosition() );
    tB.setPosition( dPos + tB.getPosition() );

    label.setPosition( dPos + label.getPosition() );
}

void sf_orGate::draw( sf::RenderTarget& RT ) const
{
    RT.draw( bodyRS );
    RT.draw( label );
    tA.draw(RT);
    tB.draw(RT);
    tOut.draw(RT);
}

bool sf_orGate::hit( sf::Vector2f pos )const
{
    sf::FloatRect rect = bodyRS.getGlobalBounds();
    if( pos.x < rect.left ) return false;
    if( pos.x > rect.left + rect.width ) return false;
    if( pos.y < rect.top ) return false;
    if( pos.y > rect.top + rect.height ) return false;
    return true;
}

void sf_orGate::reBindLambdas()
{
    tA.pSetState = [this](bool st){ setState(); };
    tB.pSetState = [this](bool st){ setState(); };
}

// sf_LogicGate
void sf_LogicGate::init( sf_terminal* Ld_pNext, std::istream& is, std::function<bool(bool,bool)> p_LogicFunc )
{
    if( !p_LogicFunc ) { if( sf_terminal::doEcho ) std::cout << "\n null pLogicFunc. Cannot init gate"; return; }
    pLogicFunc = p_LogicFunc;
    is >> name;
    bodyRS.setFillColor( sf::Color::Green );
    bodyRS.setOutlineColor( sf::Color::Black );
    bodyRS.setOutlineThickness(3.0f);
    sf::Vector2f pos;
    float W, H; is >> pos.x >> pos.y >> W  >> H;// bodyRS
    bodyRS.setSize( sf::Vector2f(W,H) );
    bodyRS.setPosition( pos );

    label.setFont( *button::pFont );
    label.setCharacterSize(14);
    label.setFillColor( sf::Color::Black );
    label.setString( name.c_str() );
    sf::Vector2f ofst; is >> ofst.x >> ofst.y;
    label.setPosition( pos + ofst );

    tA.init( [this](bool st){ updateState(); }, nullptr, is );// offset given from bodyRS
    tA.setPosition( pos + tA.getPosition() );
    tB.init( [this](bool st){ updateState(); }, nullptr, is );
    tB.setPosition( pos + tB.getPosition() );
    tOut.init( Ld_pNext, is );
    tOut.setPosition( pos + tOut.getPosition() );
    state = pLogicFunc( tA.state, tB.state );
}

void sf_LogicGate::updateState()// called by tA and tB
{
    if( !pLogicFunc ) return;
    state = pLogicFunc( tA.state, tB.state );
    if( sf_terminal::doEcho ) std::cout << "\nGate.state = " << ( state ? "on" : "off" );

    // setState only if state is changing
    if( tOut.state != state )
        tOut.setState( state );
    else if( sf_terminal::doEcho )
        std::cout << " .no change";
}

void sf_LogicGate::draw( sf::RenderTarget& RT ) const
{
    RT.draw( bodyRS );
    tA.draw(RT);
    tB.draw(RT);
    tOut.draw(RT);
}

void sf_LogicGate::setPosition( sf::Vector2f Pos )
{
    sf::Vector2f dPos = Pos - bodyRS.getPosition();// offset for moving all other members
    bodyRS.setPosition( Pos );
    label.setPosition( dPos + label.getPosition() );
    tA.setPosition( dPos + tA.getPosition() );
    tB.setPosition( dPos + tB.getPosition() );
    tOut.setPosition( dPos + tOut.getPosition() );
}

