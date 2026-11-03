#ifndef MODULE_H
#define MODULE_H

#include <packet.h>
#include <Common.h>

class Module
{
public:
    
    inline void init()
    {
        Serial.begin(BAUD_RATE);
        pinMode(2, INPUT);
    }
    
    inline void update()
    {
        bool currentState = (digitalRead(2) == HIGH);

        if (currentState && !commEnabled)
        {
            emptyStream(Serial);
        }

        commEnabled = currentState;

        if (!commEnabled) 
        {
            return;
        }

        uint8_t packetID;
        uint16_t packetWord;

        if (readPacketWait(Serial, packetID, packetWord))
        {
            sendPacket(Serial, PacketID::COMMUNICATION, Communication::ACK);
            
            if (_errorCode != 0)
            {
                sendPacket(Serial, PacketID::EXCEPTION, _errorCode);
                return;
            }
            
            switch (packetID)
            {
                case PacketID::ROUTINE:
                    {
                        processRoutine(packetWord);
                        break;
                    }
                default:
                    break;
            }
        }
    }
    
    inline void throwError(uint8_t errorCode)
    {
        _errorCode = errorCode;
    }
    
    uint8_t Batteries;
    uint16_t timer;

private:
    bool commEnabled = false;
    
    uint8_t _errorCode = 0;
    
    inline void setup()
    {
        Batteries = getAndSaveResponce(Serial, GetRequest::BATTERIES);
        uint8_t TESTVAR = getAndSaveResponce(Serial, GetRequest::TIMER);
        
        sendPacket(Serial, PacketID::COMMUNICATION, Communication::END);
    }
    
    // PACKET PROCESSING
    
    inline void processRoutine(uint16_t packetWord)
    {
        switch (packetWord)
        {
            case Routines::SETUP:
                {
                    setup();
                    return;
                }
            default:
                return;
        }
    }
};

#endif