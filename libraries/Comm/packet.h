inline void sendPacket(uint8_t packetID, uint16_t packetContent)
{
    Serial.print(packetID);
    Serial.print(" - ");
    Serial.print(highByte(packetContent));
    Serial.print(", ");
    Serial.print(lowByte(packetContent));
    Serial.println();
}