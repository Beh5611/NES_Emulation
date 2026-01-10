#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "Bus.h"
#include <cstdarg>
#include <raylib.h>
#include "cartridge.h"
#include <cstring>
#include <string>
#include "p6502.h"
#include "ppu.h"

int main(){

    const int screenWidth = 800;
    const int screenHeight = 450;
    
    std::string nes_game = "nestest.nes"; // "./Donkey Kong (World) (Rev 1).nes"
    Cartridge* cartridge = new Cartridge(nes_game);
    Bus* bus = new Bus();
    PPU ppu = PPU();
    bus->cartridge = cartridge;

    P6502 cpu = P6502(bus);
    cpu.pc = 0xC000;
    


    // running nestest.nes loop 

    // while (1){
    //     cpu.log_state();
    //     cpu.cycle();
    // }

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            ppu.generate_background();
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}



