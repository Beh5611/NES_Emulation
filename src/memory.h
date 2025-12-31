#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>
#include <cstddef>
class IMemory{
    public:
        virtual ~IMemory() {};
        virtual uint8_t read(uint16_t addr) = 0;
        virtual void write(uint16_t addr, uint8_t data) = 0;

};

class RAM : public IMemory { 
    private:
        uint8_t ram[8 * 1024];    
    public:
        RAM();
        uint8_t read(uint16_t addr) override;
        void write(uint16_t addr, uint8_t data) override;
};



#endif