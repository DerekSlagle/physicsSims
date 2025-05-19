#ifndef LVL_ELECTRICCONTROLS_H
#define LVL_ELECTRICCONTROLS_H

#include "Level.h"
//#include "../sf_terminal.h"
#include "../sf_relay2p.h"
#include "../button_types/pushButton.h"
#include "../button_types/buttonValOnHit.h"


class lvl_electricControls : public Level
{
    public:
//    const sf::Color termColorOff = sf::Color::Black;
 //   const sf::Color termColorOn = sf::Color::Yellow;

    sf_terminal t_Ln1;//, t_Ln2;// control power
    sf_terminal t_Power1;//, t_Power2;// Load power
    buttonRect cPowerButt;// turn on/off t_Ln
    buttonRect LdPowerButt;// turn on/off t_Power

 //   startStopControl ssControlLt;
 //   startStopControl ssControlRt;
 //   startStopControl ssControlUp;
    std::vector<startStopControl>  ssControlVec;

  //  sf_limitSwitch LsLeft, LsRight, LsUp;
    std::vector<sf_limitSwitch> LsVec;
 //   sf_contact con2LsLeft, con2LsRight, con2LsUp;// 2nd set of contacts for above
    std::vector<sf_contact> con2LsVec;
    buttonRect LsPowerButt;// energize com terminals above
    sf_contact revConMC[3], revConLD[3];// Left, Right, Up
    buttonRect revSelectButt;// close the 6 terminals above
    sf::Vector2f sepU_Lt, sepU_Rt, sepU_Up;// unit direction on the 3 motion legs

 //   sf_loadTerminal t_LdLt, t_LdRt, t_LdUp;// output terminals to drive motor
 //   sf_loadTerminal t_LdLt_rev, t_LdRt_rev, t_LdUp_rev;// output terminals to drive motor
    std::vector<sf_loadTerminal> loadTermVec;// to replace above: 0-2 fwd, 3-5 rev

    sf::CircleShape loadCS;// the driven "Load"
    sf::Text t_LdLabel;
    sf::Vertex rod[4];
    float Vld = 80.0f;// speed of loadCS on rod

    // TEST
    sf_contact testCon;
    sf_terminal* pTermSel = nullptr;
    void handleContactHit( sf_contact& C );
    void handleTerminalHitAll();
    std::vector<sf_terminal*> pTermBindFromOK;// ok to assign pNext. Exclusion example: contact::com.pNext = &no or &nc ONLY
    std::vector<sf_terminal*> pTermBindToOK;// Exclusion example: contact::no, nc = no linked to by com only

    // connects terminals
    std::vector<sf::Vertex> wireVec;
    void initWires3();
    // values for dir1 and dir2 sre 'V' (go vertical) and 'H'. vtxVec will contain 4 or 6 sf::Vertex at end
    void findWiringPath( sf::Vector2f pos1, char dir1, sf::Vector2f pos2, char dir2, std::vector<sf::Vertex>& vtxVec, sf::Color clr )const;

    // controls

    pushButton stopAllButt;

    buttonRect testButt;// runButt (below) holds start buttons open if testButt.sel, else it closes then opens them immediately
    pushButton runButt;// this button activates both controls start buttons
    buttonValOnHit speedStrip;

    // functions
    lvl_electricControls():Level() { std::cout << "Hello from lvl_electricControls ctor\n"; }
    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void cleanup(){}
    virtual ~lvl_electricControls() { cleanup(); }
};

#endif // LVL_ELECTRICCONTROLS_H
