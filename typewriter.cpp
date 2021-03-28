// John Eblen
// February 29, 2020
// A simple typewriter to test the pieces before building the full Busicom calculator.
#include <thread>
#include <vector>

#include "rom.h"
#include "ram.h"
#include "cpu.h"
#include "keyboard.h"
#include "tape.h"

struct Typewriter
{
    Typewriter() {
        // Only a single rom and ram are needed for this application.
        roms = std::make_shared<rom_rack>(1);
        rams = std::make_shared<ram_rack>(1);
        cpu  = std::make_shared<CPU4004>();
        kb   = std::make_shared<Keyboard>();
        tape = std::make_shared<Tape>();

        // Connect all the parts together.
        roms->at(0).set_iotype(IOTYPE::in);
        roms->at(0).connect(kb);
        rams->at(0).connect(tape);
        cpu->connect(roms);
        cpu->connect(rams);
        cpu->connect(kb);
    }

    std::shared_ptr<rom_rack> roms;
    std::shared_ptr<ram_rack> rams;
    std::shared_ptr<CPU4004>  cpu;
    std::shared_ptr<Keyboard> kb;
    std::shared_ptr<Tape>     tape;

    void run();
};

int main()
{
  Typewriter t;
  std::thread cpu_thread(&CPU4004::run, t.cpu);
  std::thread kb_thread(&Keyboard::run, t.kb);
  cpu_thread.join();
  kb_thread.join();
  return 0;
}

