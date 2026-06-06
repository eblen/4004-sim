// John Eblen
// February 29, 2020
// A simple typewriter to test the pieces before building the full Busicom calculator.
#include <cassert>
#include <thread>
#include <vector>

#include "rom.h"
#include "ram.h"
#include "cpu.h"
#include "keyboard.h"
#include "tape.h"
#include "file_io.h"

struct Typewriter
{
    Typewriter() {
	// Two roms needed to have enough ports for the keyboard and shifter.
	// The memory of the second ROM is not used.
        roms    = std::make_shared<rom_rack>(2);
        rams    = std::make_shared<ram_rack>(1);
        cpu     = std::make_shared<CPU4004>();
        kb      = std::make_shared<Keyboard>();
        tape    = std::make_shared<Tape>();

        // Connect all the parts together.
	roms->at(0).connect(IOTYPE::out, kb->get_shifter(), 0, 0);
	roms->at(0).connect(IOTYPE::out, kb->get_shifter(), 1, 1);
        roms->at(1).connect_all(IOTYPE::in, kb);
        rams->at(0).connect_all(tape);
        cpu->connect(roms);
        cpu->connect(rams);
        cpu->connect_test_port(kb);
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
  t.roms->at(0).load(ebin_to_bytes("./tw.ebin"));
  t.roms->at(1).load(ebin_to_bytes("./tw.kbmap.ebin"));

  std::thread cpu_thread(&CPU4004::run, t.cpu);
  std::thread kb_thread(&Keyboard::run, t.kb);
  cpu_thread.join();
  kb_thread.join();
  return 0;
}

