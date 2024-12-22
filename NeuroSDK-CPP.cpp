// Example tic-tac-toe game using NeuroSDK + the olcPixelGameEngine
//
#define OLC_PGE_APPLICATION
// Pixel game engine https://github.com/OneLoneCoder/olcPixelGameEngine
#include "olcPixelGameEngine.h"

#include <string>
#include "NeuroSDK/neuro-sdk.hpp"

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
    // (re)Initialize the board
    void InitBoard(){
        vBoard.resize(9, 0); // Initialize a 3x3 board with empty cells (0)
        for(int i = 0; i < 9; ++i){
            vBoard[i] = 0; // Set each cell to empty (0)
        }
        currentPlayer = 'X'; // Set the current player to X (player 1)
        gameOver = false; // Initialize the winner variable to null character (no winner yet)  
    }

    // Called once at the start of the program
	bool OnUserCreate() override
	{
        // if (!neurosdk.connect("localhost:8080")){
        //     std::cerr << "Failed to connect to Neuro" << std::endl;
        //     return false;
        // }
  //      neurosdk.gameinit();
  //      Sleep(1000);
  //      neurosdk.sendContext("You are playing tic-tac-toe! This consists of a 3 by 3 grid. You will take turns placing your mark (X or O) on the board. The first player to get three of their marks in a row, either horizontally, vertically, or diagonally, wins the game. Good luck!");
        InitBoard();
        DrawBoard();

		return true;
	}

    void DrawBoard() 
    { 
        // Draw the tic-tac-toe board here
        // Walk the board populating the X's or O's as needed
        Clear(olc::BLACK); // Clear the screen to black
    //    FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK); // Fill the screen with black
        
        int cellWidth = ScreenWidth() / 3;
        int cellHeight = ScreenHeight() / 3;

        // A smart person would of calculated these
        constexpr int xLeftPad = 20;
        constexpr int xRightPad = 70;
        constexpr int xTopPad = 15;
        constexpr int xBottomPad = 65;
        constexpr int oXOffset = 43;
        constexpr int oYOffset = 40;
        constexpr int oRadius = 30;

        //Draw tic-tac-toe grid lines
        for (int i = 1; i < 3; ++i)
        {
            FillRect(i * cellWidth, 0, 2, ScreenHeight(), olc::WHITE);
            FillRect(0, i * cellHeight, ScreenWidth(), 2, olc::WHITE);
        }

        //Draw X's and O's based on the board state
        for( int y = 0; y<3; y++)
        {
            for( int x = 0; x<3; x++)
            {
                char player = vBoard[(y*3)+x];
                if (player == 'X')
                {
                    DrawLine(x*cellWidth+xLeftPad, y*cellHeight+xTopPad+y, x*cellWidth+xRightPad, y*cellHeight+xBottomPad+y, olc::WHITE);
                    DrawLine(x*cellWidth+xRightPad, y*cellHeight+xTopPad+y, x*cellWidth+xLeftPad, y*cellHeight+xBottomPad+y, olc::WHITE);
                }   
                else if (player == 'O') 
                {
                    DrawCircle(x*cellWidth+oXOffset, y*cellHeight+oYOffset, oRadius, olc::WHITE);
                }
            }
        }

    }

	bool OnUserUpdate(float fElapsedTime) override
	{
        if( GetMouse(0).bPressed )
        {
            int x = GetMouseX() / 100;
            int y = GetMouseY() / 100;
            if (x >= 0 && x < 3 && y >= 0 && y < 3)
            {
                int index = (y * 3) + x;
                if (vBoard[index] == 0)
                {
                    vBoard[index] = currentPlayer;
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                }
            }
            // Redraw board after each move
            DrawBoard();
        }
		return true;
	}

    bool OnUserDestroy() override
    {
        neurosdk.disconnect();
		return true;
    }

private:
    NeuroSDK neurosdk;
    std::vector<uint8_t> vBoard;
    char currentPlayer;
    bool gameOver;
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