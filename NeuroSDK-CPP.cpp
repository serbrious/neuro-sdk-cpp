// Example tic-tac-toe game using NeuroSDK + the olcPixelGameEngine
//
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <string>
#include "NeuroSDK/neuro-sdk.hpp"

// Pixel game engine https://github.com/OneLoneCoder/olcPixelGameEngine


using namespace neuro;

const char* appName = "tic-tac-toe";


class TicTacToeDemo : public olc::PixelGameEngine
{
public:
	TicTacToeDemo() : neurosdk(appName)
	{
		sAppName = appName;
	}

public:
    // Called once at the start of the program
	bool OnUserCreate() override
	{
        if (!neurosdk.connect("localhost:8080")){
            std::cerr << "Failed to connect to Neuro" << std::endl;
            return false;
        }
        neurosdk.gameinit();
        Sleep(1000);
        neurosdk.sendContext("You are playing tic-tac-toe! This consists of a 3 by 3 grid. You will take turns placing your mark (X or O) on the board. The first player to get three of their marks in a row, either horizontally, vertically, or diagonally, wins the game. Good luck!");
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand()% 255));	
		return true;
	}

    bool OnUserDestroy() override
    {
        neurosdk.disconnect();
		return true;
    }

private:
    NeuroSDK neurosdk;
};


int main()
{
	TicTacToeDemo demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}



// int main() {
//     // Initialize NeuroSDK - and set our game name to 'tic-tac-toe'
//     // OR naughts and crosses if you have a bit more culture :)
//     NeuroSDK neurosdk("tic-tac-toe");

//     neurosdk.connect("localhost:8080");

//     neurosdk.gameinit();

//     Sleep(1000);

//     neurosdk.sendContext("You are playing tic-tac-toe! This consists of a 3 by 3 grid. You will take turns placing your mark (X or O) on the board. The first player to get three of their marks in a row, either horizontally, vertically, or diagonally, wins the game. Good luck!");

//     Sleep(1000);

//     std::string output;

// //    neurosdk.receive(&output);

//     std::cout << output << std::endl;;
//     std::cout << "Got: " << output.length() << "chars" << std::endl; ;

//     // Close the connection
//     neurosdk.disconnect();

//     return 0;
// }