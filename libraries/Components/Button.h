typedef void (*BtnPressed)();
typedef void (*BtnReleased)();
    
class Button
{
public:
    Button(uint8_t pin, uint16_t debounceMs = 50, bool activeLow = true) 
        : _pin(pin), 
          _debounceMs(debounceMs),
          _isPressed(false), 
          _lastRawState(false), 
          _pressedFlag(false), 
          _releasedFlag(false), 
          _activeLow(activeLow) {}

    BtnPressed onPressed = []{};
    BtnReleased onReleased = []{};
    
    void begin() const
    {
        if (_activeLow)
        {
            pinMode(_pin, INPUT_PULLUP);
        }
        else
        {
            pinMode(_pin, INPUT);
        }
    }
    
    void update()
    {
        bool rawState = digitalRead(_pin);
        if (_activeLow)
        {
            rawState = !rawState;
        }

        uint32_t now = millis();
        
        if (rawState != _lastRawState)
        {
            _lastRawState = rawState;
            _lastDebounceTime = now;
        }
        
        if ((now - _lastDebounceTime) >= _debounceMs)
        {
            if (rawState != _isPressed)
            {
                _isPressed = rawState;
                
                if (_isPressed)
                {
                    onPressed();
                    _pressTimestamp = now;
                }
                else
                {
                    onReleased();
                }
            }
        }
    }
    
    bool down() const
    {
        return _isPressed;
    }
    
    uint32_t pressTime() const
    {
        return _pressTimestamp;
    }
    
    uint32_t heldTime() const
    {
        return millis() - _pressTimestamp;
    }

private:
    uint8_t _pin;
    uint16_t _debounceMs;
    
    uint32_t _pressTimestamp = 0;
    uint32_t _pressTime = 0;
    uint32_t _lastDebounceTime = 0;
    
    bool _isPressed : 1;
    bool _lastRawState : 1;
    bool _pressedFlag : 1;
    bool _releasedFlag : 1;
    bool _activeLow : 1;
};