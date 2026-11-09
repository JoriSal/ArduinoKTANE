#include <Common.h>
#include <packet.h>

const uint8_t MAX_MODULES = 12;
uint16_t ModuleSlots = 0;

uint8_t OpenedModule = 0;

// const uint8_t ModuleEnableClock = 0; pin to clock module 4 bit counter
// const uint8_t ModuleEnableClear = 0; pin to clear module 4 bit counter
// const uint8_t ModuleEnable = 0; pin to toggle comm on or off

uint16_t getResponseTable[GetRequest::MAX_GET_REQUESTS];

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
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);

    Serial.begin(BAUD_RATE);
    Serial1.begin(BAUD_RATE);

    delay(100);
    setupBomb();
}

void loop()
{
    runRoutine(Routines::UPDATE, 6);
}

inline void setupBomb() {

    // temporary for testing the GET/RETURN
    getResponseTable[GetRequest::BATTERIES] = 60;
    getResponseTable[GetRequest::TIMER] = 62;

    //for (uint8_t module = 0; module < MAX_MODULES; module++)
    runRoutine(Routines::SETUP, 6);
}

inline void openModule(uint8_t moduleSlot)
{
    emptyStream(Serial1);
    closeModule();

    // for a later date (simple counter+decoder enable instead of one pin per module)
    /*
    for (uint8_t i = 0; i < moduleSlot; i++)
    {
        digitalWrite(ModuleEnableClock, HIGH)
        delayMicroseconds(10);
        digitalWrite(ModuleEnableClock, LOW)
        delayMicroseconds(10);
    }
    
    digitalWrite(ModuleEnable, HIGH)
    
    delay(1);
     */

    // TESTING ONLY
    digitalWrite(6, HIGH);
}

inline void closeModule()
{
    digitalWrite(6, LOW);
    //digitalWrite(ModuleEnable, LOW); - counter+decoder logic
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
    return true;
}

void throwException(uint8_t errorCode)
{
    closeModule();

    for (uint8_t i = 0; i < 12; i++)
    {
        openModule(i);
        sendPacket(Serial1, PacketID::EXCEPTION, errorCode);
    }

    while(true) {}
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
    if (packetWord >= GetRequest::MAX_GET_REQUESTS)
    {
        throwException(Exceptions::INVALID_GET);
        return;
    }
    sendPacket(Serial1, PacketID::RETURN, getResponseTable[packetWord]);
}

inline void processSetPacket(uint16_t packetWord)
{
    if (packetWord >= GetRequest::MAX_GET_REQUESTS)
    {
        throwException(Exceptions::INVALID_SET);
        return;
    }
    
    uint8_t funcPacketID;
    uint16_t funcPacketWord;

    if (!readPacketWait(Serial1, funcPacketID, funcPacketWord))
    {
        throwException(Exceptions::TIMEOUT);
        return;
    }

    getResponseTable[packetWord] = funcPacketWord;
}

inline void processFlagPacket(uint16_t packetWord)
{
    sendPacket(Serial1, PacketID::RETURN, 0);
}