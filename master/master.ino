#include <Common.h>
#include <packet.h>

const uint8_t MAX_MODULES = 12;

const uint8_t MODULE_ENABLE_DATA = 2;
const uint8_t MODULE_ENABLE_CLOCK = 3;
const uint8_t MODULE_ENABLE_LATCH = 4;

uint16_t ModuleSlots = 0;
uint16_t UnsolvedModules = 0;

uint8_t OpenedModule = 0;

uint8_t moduleTypeTemp = 0;

uint16_t getResponseTable[GetMaster::MAX_GET_REQUESTS];

inline void setupBomb();
inline void openModule(uint8_t moduleSlot);
inline void closeModule();
inline bool runRoutine(uint16_t routine, uint8_t moduleSlot);
void throwException(uint8_t errorCode);
inline void processPacket(uint8_t packetID, uint16_t packetWord);
inline void processGetPacket(uint16_t packetWord);
inline void processSetPacket(uint16_t packetWord);
inline void processFlagPacket(uint16_t packetWord);

void setup()
{
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);

    Serial.begin(BAUD_RATE);
    Serial1.begin(BAUD_RATE);

    delay(100);
    setupBomb();
}

void loop()
{
    for (uint8_t module = 1; module <= MAX_MODULES; module++)
    {
        if (!(ModuleSlots & 1 << module))
        {
            continue;
        }
        
        runRoutine(Routines::UPDATE, module);
    }
}

inline void setupBomb()
{
    // temporary for testing the GET/RETURN
    getResponseTable[GetMaster::BATTERIES] = 60;
    getResponseTable[GetMaster::TIMER] = 62;

    for (uint8_t module = 1; module <= MAX_MODULES; module++)
    {
        if (!runRoutine(Routines::SETUP, module))
        {
            continue;
        }

        ModuleSlots |= 1 << module;
        
        if (moduleTypeTemp == ModuleType::NORMAL)
        {
            UnsolvedModules |= 1 << module;
        }
        
    }
    
    if (!UnsolvedModules)
    {
        throwException(Exception::INVALID_BOMB_SETUP);
    }
}

inline void openModule(uint8_t moduleSlot)
{
    closeModule();

    emptyStream(Serial1);
    
    uint16_t mask = 1 << moduleSlot;
    
    shiftOut(MODULE_ENABLE_DATA, MODULE_ENABLE_CLOCK, MSBFIRST, mask >> 8);
    shiftOut(MODULE_ENABLE_DATA, MODULE_ENABLE_CLOCK, MSBFIRST, mask);

    digitalWrite(MODULE_ENABLE_LATCH, HIGH);
    digitalWrite(MODULE_ENABLE_LATCH, LOW);
    
    OpenedModule = moduleSlot;
    
    delay(1);
}

inline void closeModule()
{
    delay(100);
    
    shiftOut(MODULE_ENABLE_DATA, MODULE_ENABLE_CLOCK, MSBFIRST, 0);
    shiftOut(MODULE_ENABLE_DATA, MODULE_ENABLE_CLOCK, MSBFIRST, 0);

    digitalWrite(MODULE_ENABLE_LATCH, HIGH);
    digitalWrite(MODULE_ENABLE_LATCH, LOW);

    OpenedModule = 0;

    delay(100);
}

inline bool runRoutine(uint16_t routine, uint8_t moduleSlot)
{
    uint8_t packetID;
    uint16_t packetWord;

    openModule(moduleSlot);

    sendPacket(Serial1, PacketID::ROUTINE, routine);

    if (!readPacketWait(Serial1, packetID, packetWord))
    {
        closeModule();
        return false;
    }

    if (packetID != PacketID::COMMUNICATION || packetWord != Communication::ACK)
    {
        closeModule();
        return false;
    }

    while (!(packetID == PacketID::COMMUNICATION && packetWord == Communication::END))
    {
        if (!readPacketWait(Serial1, packetID, packetWord))
        {
            closeModule();
            return false;
        }

        processPacket(packetID, packetWord);
    }
    closeModule();
    return true;
}

void throwException(uint8_t errorCode)
{
    uint16_t errorMessage = OpenedModule << 8 | errorCode;
    
    for (uint8_t module = 1; module <= MAX_MODULES; module++)
    {
        openModule(module);
        sendPacket(Serial1, PacketID::EXCEPTION, errorMessage);
    }

    while(true) {}
}

inline void checkSolved()
{
    if (UnsolvedModules)
    {
        return;
    }
    
    for (uint8_t module = 1; module <= 12; module++)
    {
        openModule(module);
        sendPacket(Serial1, PacketID::ROUTINE, Routines::DEFUSE);
    }

    while(true) {} // freeze (for now)
}

// MASTER PACKET PROCESSING

inline void processPacket(uint8_t packetID, uint16_t packetWord)
{
    switch (packetID)
    {
        case PacketID::COMMUNICATION:
        {
            return;
        }
        case PacketID::GET:
        {
            processGetPacket(packetWord);
            return;
        }
        case PacketID::SET:
        {
            processSetPacket(packetWord);
            return;
        }
        case PacketID::FLAG:
        {
            processFlagPacket(packetWord);
            return;
        }
        case PacketID::EXCEPTION:
        {
            throwException(packetWord);
            return;
        }
        default:
        return;
    }
}

inline void processGetPacket(uint16_t packetWord)
{
    if (packetWord >= GetMaster::MAX_GET_REQUESTS)
    {
        throwException(Exception::INVALID_GET);
        return;
    }
    sendPacket(Serial1, PacketID::RETURN, getResponseTable[packetWord]);
}

inline void processSetPacket(uint16_t packetWord)
{
    if (packetWord >= GetMaster::MAX_GET_REQUESTS)
    {
        throwException(Exception::INVALID_SET);
        return;
    }
    
    uint8_t funcPacketID;
    uint16_t funcPacketWord;

    if (!readPacketWait(Serial1, funcPacketID, funcPacketWord))
    {
        throwException(Exception::TIMEOUT);
        return;
    }
    
    if (funcPacketID != PacketID::SET_VALUE)
    {
        throwException(Exception::INVALID_SET);
    }

    getResponseTable[packetWord] = funcPacketWord;
}

inline void processFlagPacket(uint16_t packetWord)
{
    if (packetWord & Flags::SOLVE)
    {
        UnsolvedModules &= ~(1 << OpenedModule);
        checkSolved();
    }
    sendPacket(Serial1, PacketID::RETURN, 0);
}