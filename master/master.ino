#include <Common.h>
#include <packet.h>

const uint8_t MAX_MODULES = 12;
uint16_t ModuleSlots = 0;

uint16_t getResponseTable[GetRequest::MAX_GET_REQUESTS];

inline void setupBomb() {
    
    // temporary for testing the GET/RETURN
    getResponseTable[GetRequest::BATTERIES] = 60;
    getResponseTable[GetRequest::TIMER] = 62;
    
    //for (uint8_t module = 0; module < MAX_MODULES; module++)
    runRoutine(Routines::SETUP, 6);
}

inline void closeModule(uint8_t moduleEnable)
{
    digitalWrite(moduleEnable, LOW);
}

inline bool runRoutine(uint16_t routine, uint8_t moduleEnable)
{
    uint8_t packetID;
    uint16_t packetWord;
    
    emptyStream(Serial1);

    digitalWrite(moduleEnable, HIGH);
    delay(1);

    sendPacket(Serial1, PacketID::ROUTINE, routine);
    
    if (!readPacketWait(Serial1, packetID, packetWord))
    {
        closeModule(moduleEnable);
        return false;
    }
    
    if (packetID != PacketID::COMMUNICATION || packetWord != Communication::ACK)
    {
        closeModule(moduleEnable);
        return false;
    }
    
    while (!(packetID == PacketID::COMMUNICATION && packetWord == Communication::END))
    {
        if (!readPacketWait(Serial1, packetID, packetWord))
        {
            closeModule(moduleEnable);
            return false;
        }
        
        if (packetID == PacketID::EXCEPTION)
        {
            throwException(packetWord);
            break;
        }
        processPacket(packetID, packetWord);
    }
    return true;
}

void throwException(uint8_t errorCode)
{
}

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
        case PacketID::FLAG:
        {
            processFlagPacket(packetWord);
            return;
        }
        default:
            return;
    }
}


inline void processGetPacket(uint16_t packetWord)
{
    if (packetWord < GetRequest::MAX_GET_REQUESTS)
    {
        sendPacket(Serial1, PacketID::RETURN, getResponseTable[packetWord]);
    }
    throwException(Exceptions::INVALID_GET);
}

inline void processFlagPacket(uint16_t packetWord)
{
    sendPacket(Serial1, PacketID::RETURN, 0);
}