class Module
{
public:
    Module(uint16_t moduleId) : ModuleID(moduleId)
    {
        Serial.print("Initialized module ID: ");
        Serial.println(ModuleID);
    };
    
    void recive();
    
    uint16_t ModuleID;
    uint8_t Batteries;
    uint16_t timer;
};