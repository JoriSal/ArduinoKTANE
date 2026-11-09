#ifndef PACKET_H
#define PACKET_H

enum PacketID : uint8_t
{
    ROUTINE,
    GET,
    SET,
    SET_VALUE,
    RETURN,
    COMMUNICATION,
    EXCEPTION,
    FLAG
};

enum Communication : uint16_t
{
    ACK,
    END
};

enum GetRequest : uint16_t
{
    BATTERIES,
    TIMER,
    MAX_GET_REQUESTS
};

enum Routines : uint16_t
{
    SETUP,
    UPDATE,
    EXPLODE
};

inline void emptyStream(Stream &stream)
{
    while (stream.available() > 0) stream.read();
}

inline void sendPacket(Stream &stream, uint8_t packetID, uint16_t packetContent)
{
    stream.write(packetID);
    stream.write(highByte(packetContent));
    stream.write(lowByte(packetContent));
    stream.flush();
}

inline bool readPacket(Stream &stream, uint8_t &packetID, uint16_t &packetContent)
{
    if (stream.available() < 3)
    {
        return false;
    }

    packetID = stream.read();
    uint8_t highContentByte = stream.read();
    uint8_t lowContentByte = stream.read();

    packetContent = (highContentByte << 8) | lowContentByte;

    return true;
}

inline bool readPacketWait(Stream &stream, uint8_t &packetID, uint16_t &packetContent)
{
    uint32_t startTime = millis();
    while (stream.available() < 3)
    {
        if (millis() - startTime > 50) return false;
    }
    
    return readPacket(stream, packetID, packetContent);
}

inline uint16_t getAndSaveResponse(Stream &stream, uint16_t packetContent)
{
    sendPacket(stream, PacketID::GET, packetContent);
    
    uint8_t packetID = 0;
    
    packetContent = 0;
    
    if (readPacketWait(stream, packetID, packetContent) && packetID == PacketID::RETURN)
    {
        return packetContent;
    }
    
    return packetContent;
}

#endif