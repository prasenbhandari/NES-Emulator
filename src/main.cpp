
#include "core/Bus.h"
#include "core/CPU.h"


int main() {
    Bus bus;
    CPU cpu;

    bus.connect_cpu(&cpu);
    cpu.connect_bus(&bus);

    return 0;
}
