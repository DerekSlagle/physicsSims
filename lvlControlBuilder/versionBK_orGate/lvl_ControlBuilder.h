#ifndef LVL_CONTROLBUILDER_H
#define LVL_CONTROLBUILDER_H

#include "Level.h"
#include "../sf_relay2p.h"
#include "../button_types/radioButton.h"
#include "../button_types/buttonValOnHit.h"

class lvl_ControlBuilder : public Level
{
    public:
//    sf_contact* pContactSel = nullptr;
    sf_terminal* pTermSel = nullptr;// use for all sf_terminal types
    std::vector<sf_terminal*> pTermBindFromOK;// ok to assign pNext. Exclusion example: contact::com.pNext = &no or &nc ONLY
    // 'A' = terminal, 'B' = contact, 'C' = loadTerm, 'D' = pbNO, 'E' = pbNC
    // 'F' = orGate, 'G' = bus2out, 'H' = limitSwitch
    std::vector<char> typeOrderVec;// records push_back order by type for save load

    std::vector<sf_terminal*> pTermBindToOK;// Exclusion example: contact::no, nc = no linked to by com only

    // controls
    buttonRect addTerminalButt;
    buttonRect addContactButt;
    buttonRect addLoadTermButt;
    buttonRect addPushButtNoButt, addPushButtNcButt;
    buttonRect addOrGateButt;
    buttonRect addBus2outButt;
    buttonRect addLimitSwitchButt;
    std::vector<buttonRect*> pAddButtVec;// to automate processing of above
    radioButton modeRadioButt;
    void setRBmode( size_t mode );
    buttonRect drawWiresButt;// To draw wires, or not to draw wires? A: drawWiresButt.sel
    bool init_controls();

    // supply "circuits"
    sf_terminal t_ControlPower;
    sf_terminal t_LoadPower;
    buttonRect cPowerButt;// toggle t_ControlPower
    buttonRect LdPowerButt;// toggle t_LoadPower
    // save / load
    pushButton saveButt, loadButt;

    // template instances
    sf_loadTerminal LdTerm_toCopy;// terminal type
    sf_pushButton pbNO_toCopy, pbNC_toCopy;// not a terminal type
    sf_pushButton* pPushButtSel = nullptr;// for calling setPosition() in "move" mode
    sf_orGate orGate_toCopy;// not a terminal type
    sf_orGate* pOrGateSel = nullptr;// for calling setPosition() in "move" mode
    sf_bus2out bus2out_toCopy;// a terminal type: sf_terminal -> bus2 -> bus2out
    sf_limitSwitch limitSwitch_toCopy;
    sf_limitSwitch *pLS_moveCon = nullptr, *pLS_moveSw = nullptr;// to select and move the |\0 part of the image

    // containers
    std::vector<startStopControl>  ssControlVec;// composite
    std::vector<sf_terminal> terminalVec;// base type
    unsigned int termVecCapacity = 40;

    std::vector<sf_contact> contactVec;// terminal type: from terminal
    unsigned int conVecCapacity = 40;// also used for loadTermVec capacity, pushButtVec, orGateVec
    std::vector<sf_limitSwitch> LimitSwitchVec;// terminal type: from contact
    std::vector<sf_loadTerminal> loadTermVec;// terminal type
    std::vector<sf_pushButton> pushButtVec;// not a terminal type
    std::vector<sf_orGate> orGateVec;// not a terminal type
    std::vector<sf_bus2out> bus2outVec;// terminal type

    // a path to traverse defined by limit switch positions
    std::vector<sf::Vertex> loadPath;// may be open or closed. Click on 1st LS when in program mode to close
    std::vector<sf::Vector2f> pathDir;// unit vector in each forward direction
    sf::CircleShape loadCS;// to animate along path
    void addToLoadPath( sf_limitSwitch& LS );
    void finishLoadPath();
    bool pathIsComplete = false;
    float loadSpeed = 50.0f;
    buttonValOnHit speedStrip;
    std::vector<sf_terminal*> pTermFwdLeads, pTermRevLeads;// one for each pathDir, reverse optional
    pushButton idFwdLeadButt, idRevLeadButt;// active in program mode once pathIsComplete then hidden once all are ID

    //
    sf::Color contactColor;
    sf::Color termSelColor;
    sf::Color contactSelColor;
    sf::Color moveSelColor;

    virtual bool init();
    virtual bool handleEvent( sf::Event& rEvent );
    virtual void update( float dt );
    virtual void draw( sf::RenderTarget& RT ) const;

    void handleAddDevice();
    void handleMovingDevice();
    void operateDevices();
    void programDevices();

    void saveControls()const;
    bool loadControls();// file read may fail

    void cleanup(){}

    lvl_ControlBuilder(){}
    virtual ~lvl_ControlBuilder(){ cleanup(); }
};

#endif // LVL_CONTROLBUILDER_H
