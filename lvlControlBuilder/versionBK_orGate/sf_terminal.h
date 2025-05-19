#ifndef SF_TERMINAL_H
#define SF_TERMINAL_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "button_types/buttonRect.h"
#include "button_types/pushButton.h"
//#include "terminal.h"

class sf_terminal
{
    public:
    static const sf::Color termColorOff;
    static const sf::Color termColorOn;
    static bool doEcho;
    static bool doDrawWires;

    std::string name;
    bool state = false;
    sf_terminal* pNext = nullptr;
    int bindIdx = -1;// supports save / load of controls

    sf::CircleShape tmCS;
    sf::Text tmLabel;
    sf::Vertex wireToNext[2];

    void init( sf_terminal* p_next, std::istream& is );
    void init( sf_terminal* p_next, const std::string& name, float R, float posX, float posY, float offX, float offY );
    void init( sf_terminal* p_next, const sf_terminal& copyTerm ) { *this = copyTerm; pNext = p_next; }
    virtual void setState( bool st );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( sf::Vector2f pos );// call only after init()
    sf::Vector2f getPosition()const { return tmCS.getPosition(); }

    virtual bool pos_isOver( sf::Vector2f pos )const;// overload in bus2out

    void drawWire( sf::RenderTarget& RT ) const;// call if wires are to be drawn
    void set_pNext( sf_terminal* p_Next );// also sets wireToNext positions
    void update_wirePosition();// only sets wireToNext positions

    sf_terminal(){ state = false; pNext = nullptr; }
    virtual ~sf_terminal(){}
};

class sf_loadTerminal : public sf_terminal
{
    public:
    std::function<void(bool)> pSetState = nullptr;// can now call setState(bool) on any type eg relay
    void init( std::function<void(bool)> pSetFunc, sf_terminal* p_next, std::istream& is );
    virtual void setState( bool st );
};

class sf_bus2 : public sf_terminal// one terminal in ( *this ) and 2 out
{
    public:
    sf_terminal *pTermA, *pTermB;
    virtual void init( sf_terminal* p_termA, sf_terminal* p_termB, std::istream& is );
    virtual void setState( bool st );
};

class sf_busN : public sf_terminal// one terminal in ( *this ) and N out
{
    public:
    std::vector<sf_terminal*> pTermVec;
    void init( const std::vector<sf_terminal*>& ptVec, std::istream& is );
    void init( std::istream& is ){ sf_terminal::init( nullptr, is ); }
    virtual void setState( bool st );
};

// A "bus2" with output terminals and arrows to them from *this ( bus2 is a terminal)
class sf_bus2out : public sf_bus2// one terminal in ( *this ) and 2 out
{
    public:
    static sf::Color arrowColor;
//    sf_terminal *pTermA, *pTermB;// inherited
    sf_terminal tOutA, tOutB;
    sf::Vertex arrows[9];
    virtual void init( sf_terminal* p_termA, sf_terminal* p_termB, std::istream& is );
 //   virtual void setState( bool st );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual bool pos_isOver( sf::Vector2f pos )const;// overload in base
    virtual void setPosition( sf::Vector2f pos );
};

class sf_pushButton
{
    public:
    std::string name;
    bool state = false;// true if closed
    sf_terminal Ln, Ld;
    sf::Vertex line[2];// connects Ln and Ld graphically
//    buttonRect pbButt;
    pushButton pbButt;

    void init( bool st, sf_terminal* Ld_pNext, std::istream& is );
    void setState( bool st );
    void draw( sf::RenderTarget& RT ) const;
    sf::Vector2f getPosition()const { return Ln.getPosition(); }// at Ln terminal
    void setPosition( sf::Vector2f Pos );// at Ln terminal
    sf_pushButton(){ state = false; Ln.pNext = Ld.pNext = nullptr; }
};

// energy flow is from com to pTerm_no or pTerm_nc
class sf_contact : public sf_terminal// inherited sf_contact is the "coil". It activates the contact
{
    public:
//    std::string name;// inherited
//    terminal* pNext;// inherited - points to next contact
//    bool state = false;// inherited
    sf::RectangleShape bodyRS;// encloses terminals
    sf_terminal com;
    sf_terminal no, nc;
 //   sf::Vertex line[2];// connects com and no or nc graphically
    void init( sf_terminal* pTermNo, sf_terminal* pTermNc, std::istream& is, sf_terminal* p_Next = nullptr );
    void init( sf_terminal* pTermNo, sf_terminal* pTermNc, sf::Vector2f Pos, float W, char facing, sf::Color clr, const std::string& name, sf_terminal* p_Next = nullptr );
    virtual void setState( bool st );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( sf::Vector2f pos );// call only after init()
    virtual sf::Vector2f getPosition()const { return tmCS.getPosition(); }

    bool pos_isOver( sf::Vector2f pos, sf_terminal*& rpTermOver );
    sf_contact& operator=( const sf_contact& C );
 //   sf_contact(){ com.pNext = &nc; }
    sf_contact(){ com.set_pNext( &nc ); }
    sf_contact( const sf_contact& C ){ *this = C; }
};

class sf_limitSwitch : public sf_contact
{
    public:
    sf::Vertex vtx[3];
    sf::CircleShape cs;

    float Wsw, Hsw;// of switch image
    float Xopen = 0.0f;// distance from vtx[2] to actuate
    sf::Vector2f Nu;// unit vector in direction of operation. Replaces int dir
    void setNu( sf::Vector2f n );

 //   int dir = 1;// 1: closes when x > Xopen, -1: closes when x < Xopen
    void update( float x );// impose a barrier value
    void update( sf::Vector2f pos );// must pass within Xopen to activate

    void init( std::istream& is, sf_terminal* pTermNo, sf_terminal* pTermNc, sf_terminal* p_Next = nullptr );
    virtual void setState( bool st );
    virtual void draw( sf::RenderTarget& RT ) const;
    virtual void setPosition( sf::Vector2f pos );// call only after init()
    // for moving each part
    void setContactPosition( sf::Vector2f pos );
    void setSwitchPosition( sf::Vector2f pos );
    sf::Vector2f getSwitchPosition()const{ return cs.getPosition(); }
    bool hitSwitch( sf::Vector2f P )const;// bounding rectangle from vtx
};

class sf_orGate
{
public:
    std::string name;
 //   bool state = false;// replace with getter function
    bool getState()const{ return tA.state || tB.state; }
    sf::RectangleShape bodyRS;// encloses teminals
    sf::Text label;
    sf_loadTerminal tA, tB;// inputs
    sf_terminal tOut;// output
    void setState();// called by tA and tB. state = tA.state || tB.state
    void init( sf_terminal* Ld_pNext, std::istream& is );
    void setPosition( sf::Vector2f Pos );
    sf::Vector2f getPosition()const { return bodyRS.getPosition(); }
    void draw( sf::RenderTarget& RT ) const;
    bool hit( sf::Vector2f pos )const;
    void reBindLambdas();
    sf_orGate(){}
    sf_orGate( const sf_orGate& OG ){ *this = OG; reBindLambdas(); }
};

class sf_LogicGate
{
public:
    std::string name;
    bool state = false;
    sf::RectangleShape bodyRS;// encloses teminals
    sf::Text label;
    sf_loadTerminal tA, tB;// inputs
    sf_terminal tOut;// output
    std::function<bool(bool,bool)> pLogicFunc = nullptr;// used in updateState()
    void updateState();// called by tA and tB. state = pLogicFunc( tA.state, tB.state )
    void init( sf_terminal* Ld_pNext, std::istream& is, std::function<bool(bool,bool)> p_LogicFunc );
    void draw( sf::RenderTarget& RT ) const;
    sf::Vector2f getPosition()const { return bodyRS.getPosition(); }
    void setPosition( sf::Vector2f Pos );
};

#endif // SF_TERMINAL_H
