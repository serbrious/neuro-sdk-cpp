// NeuroSDK-CPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include "neuro-sdk.hpp"

using namespace neuro;

int main() {
    // Initialize NeuroSDK - and set our game name to 'tic-tac-toe'
    // OR naughts and crosses if you have a bit more culture :)
    NeuroSDK neurosdk("tic-tac-toe");

    neurosdk.connect("localhost:8080");

    neurosdk.gameinit();

    Sleep(1000);

    neurosdk.sendContext("You are playing tic-tac-toe! This consists of a 3 by 3 grid. You will take turns placing your mark (X or O) on the board. The first player to get three of their marks in a row, either horizontally, vertically, or diagonally, wins the game. Good luck!");

    Sleep(1000);

    std::string output;

//    neurosdk.receive(&output);

    std::cout << output << std::endl;;
    std::cout << "Got: " << output.length() << "chars" << std::endl; ;

    // Close the connection
    neurosdk.disconnect();

    return 0;
}