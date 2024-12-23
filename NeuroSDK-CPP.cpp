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
        movesMade = 0;
    }

    // Called once at the start of the program
	bool OnUserCreate() override
	{
        if (!neurosdk.connect("localhost:8000")){
            std::cerr << "Failed to connect to Neuro" << std::endl;
            return false;
        }

        neurosdk.gameinit();
        Sleep(100);
        neurosdk.sendContext("You are playing tic-tac-toe! This consists of a 3 by 3 grid. You will take turns placing your mark (X or O) on the board. The first player to get three of their marks in a row, either horizontally, vertically, or diagonally, wins the game. Good luck!");

        InitBoard();
        DrawBoard();
		return true;
	}

    void indexToXY(const int index, int *x, int *y) {
        // Convert the index to 2D coordinates
        *x = index % 3;
        *y = index / 3;
    }

    void DrawBoard() 
    { 
        // Draw the tic-tac-toe board here
        // Walk the board populating the X's or O's as needed
        Clear(olc::BLACK); // Clear the screen to black       
        int cellWidth = ScreenWidth() / 3;
        int cellHeight = ScreenHeight() / 3;

        // Yes I know its a bit of an odd way to do this, but calculate our drawing positions with percentages...
        int xLeftPad = (int)(float)(cellWidth*0.1);
        int xRightPad = (int)(float)(cellWidth*0.9);
        int xTopPad = (int)(float)(cellHeight*0.1);
        int xBottomPad = (int)(float)(cellHeight*0.9);
        int oXOffset = (int)(float)(cellWidth*0.5); // aka /2
        int oYOffset = (int)(float)(cellHeight*0.5); // aka /2
        int oRadius = (int)(float)(cellHeight*0.4);

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

        if( gameOver ) {
            // Draw the winning line
            if ( movesMade<9 ) {          // Display the winner message
                FillRect(0, ScreenHeight()/2-50, ScreenWidth(), 100, olc::BLACK);
                DrawString(ScreenWidth()/2-50, ScreenHeight()/2-10, "Player " + std::string(1, currentPlayer) + " wins!", olc::WHITE);
                DrawString(ScreenWidth()/2-50, ScreenHeight()/2+10, "Press R to restart", olc::WHITE);

                // Draw the winning line
                int winX1, winY1, winX2, winY2;
                indexToXY(winT, &winX1, &winY1 );
                indexToXY(winB, &winX2, &winY2 );

                DrawLine(winX1*cellWidth+oXOffset, winY1*cellHeight+oYOffset, winX2*cellWidth+oXOffset, winY2*cellHeight+oYOffset, olc::WHITE);
            }
            else {
                FillRect(0, ScreenHeight()/2-50, ScreenWidth(), 100, olc::BLACK);
                DrawString(ScreenWidth()/2-50, ScreenHeight()/2-10, "It's a tie!", olc::WHITE);
                DrawString(ScreenWidth()/2-50, ScreenHeight()/2+10, "Press R to restart", olc::WHITE);
            }
        }

    }

    bool checkWin()
    {
        // There is probably a smarter way to do this, but it works for now.
        for( int i=0; i<3; i++ )
        {
            if( vBoard[i*3] != 0 && vBoard[i*3] == vBoard[i*3+1] && vBoard[i*3+1] == vBoard[i*3+2] )
            {   // Horizontal win
                winT = i*3;
                winB = i*3+2;
                return true;
            }
            if( vBoard[i] != 0 && vBoard[i] == vBoard[i+3] && vBoard[i+3] == vBoard[i+6] )
            {   // Vertical win
                winT = i;
                winB = i+6;
                return true;
            }
        }
        if( vBoard[0] != 0 && vBoard[0] == vBoard[4] && vBoard[4] == vBoard[8] )
        {   // Diagonal win from top-left to bottom-right   
            winT = 0;
            winB = 8;
            return true;    
        }
        if( vBoard[2] != 0 && vBoard[2] == vBoard[4] && vBoard[4] == vBoard[6] )
        {   // Diagonal win from top-right to bottom-left
            winT = 2;
            winB = 6;
            return true;    
        }
        return false;    
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
                    movesMade++;
                    vBoard[index] = currentPlayer;
                    if( movesMade >=9 ) { // Check for a tie after 9 moves
                        gameOver = true;
                    } else { // Check for win after each move
                        gameOver = checkWin();
                    }
                    if (!gameOver)
                        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                }
            }
            // Redraw board after each move
            DrawBoard();

            neurosdk.unregisterAction("play");

            Action action("play","Place an O in the specified cell.","");
            std::vector< std::string > availableCells;
            // Walk the board (the old way)   
            for (int i = 0; i < 9; i++)
            {
                if (vBoard[i] == 0) {
                    availableCells.push_back(cellToName(i));
                }
            }
            action.SetSchemaFromArray("cell",availableCells);
            neurosdk.registerAction(action);

            std::vector<std::string> validActions;
            validActions.push_back("play");

            neurosdk.forceAction("game is still under way","Its your turn, please make a move",validActions);

        }
        if( gameOver && GetKey(olc::R).bPressed ) {
            InitBoard();
            DrawBoard(); // Redraw the board after resetting
        }
		return true;
	}

    bool OnUserDestroy() override
    {
        neurosdk.disconnect();
		return true;
    }

private:
    // Yeah, I know - but I'm not on the clock so you get it the hammer and nails way.
    std::string cellToName(int cell) {
        // convert cell index to a string representation
        // on the from [top, middle, bottom] [left,middle,right]
        switch(cell) {
            case 0: return "top-left";
            case 1: return "top-middle";
            case 2: return "top-right";
            case 3: return "middle-left";
            case 4: return "middle-middle";
            case 5: return "middle-right";
            case 6: return "bottom-left";
            case 7: return "bottom-middle";
            case 8: return "bottom-right";
            default: return "unknown";
        }     
    }

    // Inverse of cellToName
    int nameToCell(std::string name) {
        if (name == "top-left") return 0;
        else if (name == "top-middle") return 1;
        else if (name == "top-right") return 2;
        else if (name == "middle-left") return 3;
        else if (name == "middle-middle") return 4;
        else if (name == "middle-right") return 5;
        else if (name == "bottom-left") return 6;
        else if (name == "bottom-middle") return 7;
        else if (name == "bottom-right") return 8;
        else return -1; // Invalid cell name
    }

    NeuroSDK neurosdk;
    std::vector<uint8_t> vBoard;
    char currentPlayer;
    bool gameOver;
    int movesMade;
    int winT;  // Left hand/top most square on a win
    int winB;  // Right hand/bottom most square on a win

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