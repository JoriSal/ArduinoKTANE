inline void sendPacket(uint8_t packetID, uint16_t packetContent)
{
    Serial.write(packetID);
    Serial.write(highByte(packetContent));
    Serial.write(lowByte(packetContent));
}

inline bool readPacket(uint8_t &packetID, uint16_t &packetContent)
{
    if (Serial.available() < 3)
    {
        return false;
    } 
    
    packetID = Serial.read();
    
    uint8_t highContentByte = Serial.read();
    uint8_t lowContentByte = Serial.read();
        
    packetContent = (highContentByte << 8) | lowContentByte;
    
    return true;
}