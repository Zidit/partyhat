#ifndef ARDUINOWRAPPER_H_INCLUDED
#define ARDUINOWRAPPER_H_INCLUDED

#include "taskmanager.h"
#include "serial.h"

class Stream
{
    public:
        Stream (uart &serial)
        {
            _serial = &serial;
        }
        bool available()
        {
            return _serial->dataAvailable();
        }
        uint8_t read()
        {
            return _serial->getChar();
        }
        void flush()
        {
            _serial->flush();

        }
        void write(uint8_t val)
        {
            _serial->sendChar(val);
        }

    private:
        uart* _serial;
};


inline uint32_t millis() { return taskManager::getTimeMs();}



#endif // ARDUINOWRAPPER_H_INCLUDED
