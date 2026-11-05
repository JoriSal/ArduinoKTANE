#ifndef MODULE_H
#define MODULE_H

#include <Common.h>
#include <packet.h>

class ModuleClass
{
public:
    inline void init()
    {
        Serial.begin(BAUD_RATE);
        pinMode(2, INPUT);
    }
    
    inline void init(uint8_t maxAttributes, uint8_t maxSyncs)
    {
        init();
        _maxAttributes = maxAttributes;
        _maxSyncs = maxSyncs;
        
        if (_maxAttributes > 0)
        {
            _attributes = new Binding[_maxAttributes];
        }
        
        if (_maxSyncs > 0)
        {
            _syncs = new Binding[_maxSyncs];
        }
    }
    
    void attribute(uint16_t &variable, uint16_t getRequest)
    {
        if (_attributeCount < _maxAttributes)
        {
            _attributes[_attributeCount] = { &variable, getRequest };
            _attributeCount++;
        }
    }

    void sync(uint16_t &variable, uint16_t getRequest)
    {
        if (_syncCount < _maxSyncs)
        {
            _syncs[_syncCount] = { &variable, getRequest };
            _syncCount++;
        }
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
                        processRoutinePacket(packetWord);
                        break;
                    }
                default:
                    break;
            }
        }
    }
    
    inline void strike()
    {
        _flags |= 1 << Flags::STRIKE;
    }
    
    inline void detonate()
    {
        _flags |= 1 << Flags::DETONATE;
    }

    inline void throwError(uint8_t errorCode)
    {
        _errorCode = errorCode;
    }
    
private:
    bool commEnabled = false;
    uint16_t _flags = 0;
    uint8_t _errorCode = 0;
    
    struct Binding {
        uint16_t* varPointer;
        uint16_t getRequest;
    };
    
    Binding* _attributes;
    uint8_t _maxAttributes = 0;
    uint8_t _attributeCount = 0;
    
    Binding* _syncs;
    uint8_t _maxSyncs = 0;
    uint8_t _syncCount = 0;
    
    inline void _setup()
    {
        for (uint8_t i = 0; i < _attributeCount; i++)
        {
            *(_attributes[i].varPointer) = getAndSaveResponce(Serial, _attributes[i].getRequest);
        }
        
        sendPacket(Serial, PacketID::COMMUNICATION, Communication::END);
    }
    
    inline void _update()
    {
        uint8_t packetID;
        uint16_t packetWord;
        
        if (_flags)
        {
            sendPacket(Serial, PacketID::FLAG, _flags);
            if (readPacketWait(Serial, packetID, packetWord) && packetID == PacketID::RETURN)
            {
                _flags = packetWord;
            }
        }
        
        for (uint8_t i = 0; i < _syncCount; i++)
        {
            *(_syncs[i].varPointer) = getAndSaveResponce(Serial, _syncs[i].getRequest);
        }
        
        sendPacket(Serial, PacketID::COMMUNICATION, Communication::END);
    }
    
    // PACKET PROCESSING
    
    inline void processRoutinePacket(uint16_t packetWord)
    {
        switch (packetWord)
        {
            case Routines::SETUP:
                {
                    _setup();
                    return;
                }
            case Routines::UPDATE:
                {
                    _update();
                    return;
                }
            case Routines::EXPLODE:
                {
                    // ADD EXPLOSION LOGIC (like the coloured led or something)
                    return;
                }
            default:
                return;
        }
    }
};

inline ModuleClass Module;

#endif