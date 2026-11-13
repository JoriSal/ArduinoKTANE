#ifndef MODULE_H
#define MODULE_H

#include <Common.h>
#include <packet.h>

typedef void (*ExceptionCallback)(uint16_t);

class ModuleClass
{
public:
    
    bool solved = false;
    
    ExceptionCallback onException = [](uint16_t) {};

    inline void init(uint8_t moduleType, uint8_t maxConfigurations = 0, uint8_t maxSyncs = 0, uint8_t maxReports = 0)
    {
        _moduleType = moduleType;
        
        Serial.begin(BAUD_RATE);
        pinMode(2, INPUT);
        
        _maxConfigurations = maxConfigurations;
        _maxSyncs = maxSyncs;
        _maxReports = maxReports;
        
        if (_maxConfigurations > 0)
        {
            _configurations = new Binding[_maxConfigurations];
        }
        
        if (_maxSyncs > 0)
        {
            _syncs = new Binding[_maxSyncs]; 
        }

		if (_maxReports > 0)
		{
			_reports = new Binding[_maxReports];
    	}
    }
    
    void configuration(uint16_t &variable, uint16_t getRequest)
    {
        if (_configurationCount < _maxConfigurations)
        {
            _configurations[_configurationCount] = { &variable, getRequest };
            _configurationCount++;
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
        
    void report(uint16_t &variable, uint16_t getRequest)
    {
        if (_reportCount < _maxReports)
        {
            _reports[_reportCount] = { &variable, getRequest };
            _reportCount++;
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
                case PacketID::EXCEPTION:
                    {
                        onException(packetWord);
                        break;
                    }
                
                default:
                    break;
            }
        }
    }
    
    inline void solve()
    {
        if (!solved)
        {
            _flags |= Flags::SOLVE;
            solved = true;
        }
    }
    
    inline void strike()
    {
        _flags |= Flags::STRIKE;
    }
    
    inline void detonate()
    {
        _flags |= Flags::DETONATE;
    }

    inline void throwError(uint8_t errorCode)
    {
        _errorCode = errorCode;
    }
    
    bool commEnabled = false; // public for testing purposes only
    
private:
    // bool commEnabled = false;
    uint16_t _flags = 0;
    uint8_t _errorCode = 0;
    uint8_t _moduleType = ModuleType::NO_MODULE;
    
    struct Binding {
        uint16_t* varPointer;
        uint16_t getRequest;
    };
    
    Binding* _configurations;
    uint8_t _maxConfigurations = 0;
    uint8_t _configurationCount = 0;
    
    Binding* _syncs;
    uint8_t _maxSyncs = 0;
    uint8_t _syncCount = 0;

	Binding* _reports;
	uint8_t _maxReports = 0;
	uint8_t _reportCount = 0;
    
    inline void _setup()
    {
        for (uint8_t i = 0; i < _configurationCount; i++)
        {
            *(_configurations[i].varPointer) = getAndSaveResponse(Serial, _configurations[i].getRequest);
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
            *(_syncs[i].varPointer) = getAndSaveResponse(Serial, _syncs[i].getRequest);
        }
        
        for (uint8_t i = 0; i < _reportCount; i++)
        {
            sendPacket(Serial, PacketID::SET, _reports[i].getRequest);
            sendPacket(Serial, PacketID::SET_VALUE, *(_reports[i].varPointer));
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
			case Routines::DEFUSE:
				{
					// ADD DEFUSE LOGIC (blink the status LED green)
					return;
				}
            default:
                {
                    throwError(Exception::INVALID_ROUTINE);
                    return;
                }
        }
    }
};

inline ModuleClass Module;

#endif