// John Eblen
// October 26, 2022
// A machine for playing "moo," a simple but entertaining guessing game
#include <thread>
#include <vector>

#include "rom.h"
#include "ram.h"
#include "cpu.h"
#include "hexpad.h"
#include "hextape.h"
#include "file_io.h"

struct MooGame
{
    MooGame() {
        // Only a single rom and ram are needed for this application.
        roms  = std::make_shared<rom_rack>(2);
        rams  = std::make_shared<ram_rack>(1);
        cpu   = std::make_shared<CPU4004>();
        hpad  = std::make_shared<Hexpad>();
        htape = std::make_shared<HexTape>();

        // Connect all the parts together.
        roms->at(0).set_iotype(IOTYPE::in);
        roms->at(0).connect(hpad);
        rams->at(0).connect(htape);
        cpu->connect(roms);
        cpu->connect(rams);
        cpu->connect(hpad);
    }

    std::shared_ptr<rom_rack> roms;
    std::shared_ptr<ram_rack> rams;
    std::shared_ptr<CPU4004>  cpu;
    std::shared_ptr<Hexpad>   hpad;
    std::shared_ptr<HexTape>  htape;

    void run();
};

int main()
{
  MooGame mg;
  mg.roms->at(0).load(ebin_to_bytes("./moo.ebin"));
  mg.roms->at(1).load(ebin_to_bytes("./moo.keys.ebin"));

  std::thread cpu_thread(&CPU4004::run, mg.cpu);
  std::thread hpad_thread(&Hexpad::run, mg.hpad);
  cpu_thread.join();
  hpad_thread.join();
  return 0;
}

