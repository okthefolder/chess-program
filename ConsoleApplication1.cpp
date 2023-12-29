#include <iostream>
#include <list>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <Windowsx.h>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <limits>
#include <tuple>
#include <thread>
#include <functional>
#include <omp.h>
#include <unordered_map>
#include <functional>

//std::list<std::vector<int>> listing_all_possible_moves(int turn, std::vector<int> board, int care_about_checks);
//std::vector<int> check_if_move_is_legal(int n, std::vector<int> possible_moves, std::vector<int>& board, int turn);
int search(int board[], int turn, int depth, int starting_depth, int turnn, int aspiration_window, int aspiration_window_depth);
std::vector<int> best_evaluation = { 0,0,0 };
std::vector<int> best_evaluations = {};
std::tuple<int, int> finding_best_move(int board[], int turn, int depth, std::tuple<int, int> MoveToMake);
void make_move(int board[], int selectedPieceIndex, int clickedPieceIndex);
std::uint64_t calculateBoardHash(int board[72], int turn);
int computer = 0;
int moves_made = 0;
int selectedPieceIndex = -1;
int turn = 2;
float endgame_state = 0;
int found_lookup = 0;

std::uint64_t pieceKeys[832];
std::uint64_t sideKey;

struct TranspositionTableEntry {
    std::uint64_t hash;
    //int best_move_piece;
    //int best_move_to;
    int age;
    int depth;
    int evaluation;
    // Additional information as needed

    // Default constructor
    TranspositionTableEntry() : hash(0), age(0), depth(0), evaluation(0) {}

    // Parameterized constructor
    TranspositionTableEntry(std::uint64_t h, int a, int d, int e) : hash(h), age(a), depth(d), evaluation(e) {}
};

std::unordered_map<std::uint64_t, TranspositionTableEntry> transpositionTable;
void performAgeBasedDeletion(std::unordered_map<std::uint64_t, TranspositionTableEntry>& table, int maxAge);
int visible_board[72] = {

      8,9,10,11,12,10,9,8,
        7,7,7,7,7,7,7,7,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,
        2,3,4,5,6,4,3,2,
        /*8, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 6, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        12, 0, 0, 0, 0, 0, 0, 0,*/

        0,0,  0,0,0, 0,0,0 };

std::vector<int> piece_evaluation_board_1 = {
         0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0,0,0,
        -5,-10,-10,-5,-5,-10,-10,-5,
        -10,-5, 0,16,16, 0,-5,-10,
        -5, 0,12, 8, 8, 12,0,-5,
         0, 10, 6, 0, 0, 6, 10,0,
         10, 5, 0, -5, -5, 0, 5, 10,
         0, 0, 0, 0, 0, 0, 0, 0
};
std::vector<int> piece_evaluation_board_7 = {
 0, 0, 0, 0, 0, 0, 0, 0,
 10, 5, 0,-5,-5, 0, 5,10,
  0,10, 6, 0, 0, 6,10, 0,
 -5, 0,12, 8, 8,12, 0,-5,
-10,-5, 0,16,16, 0,-5,-10,
 -5,-10,-10,-5,-5,-10,-10,-5,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};
std::vector<int> piece_evaluation_board_2 = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
};
std::vector<int> piece_evaluation_board_8 = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
};
std::vector<int> piece_evaluation_board_3 = {
        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 8, 8, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,
};
std::vector<int> piece_evaluation_board_9 = {
        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 8, 8, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,
};
std::vector<int> piece_evaluation_board_4 = {
        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,
};
std::vector<int> piece_evaluation_board_10 = {
        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,
};
std::vector<int> piece_evaluation_board_5 = {
        -5, -3, 0, 0, 0, 0, -3, -5,
        -3, 0, 0, 0, 0, 0, 0, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        -3, 0, 0, 0, 0, 0, 0, -3,
        -5, -3, 0, 0, 0, 0, -3, -5,
};
std::vector<int> piece_evaluation_board_11 = {
        -5, -3, 0, 0, 0, 0, -3, -5,
        -3, 0, 0, 0, 0, 0, 0, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        -3, 0, 0, 0, 0, 0, 0, -3,
        -5, -3, 0, 0, 0, 0, -3, -5,
};
std::vector<int> piece_evaluation_board_6 = {
        -4, -6, -8, -10, -10, -8, -6, -4,
        -2, -4, -6, -8, -8, -6, -4, -2,
        0, -2, -4, -6, -6, -4, 0, 0,
        2, 0, -2, -4, -4, -2, 0, 2,
        4, 2, 0, -2, -2, 0, 2, 4,
        6, 4, 2, 0, 0, 2, 4, 6,
        8, 6, 4, 2, 2, 4, 6, 8,
        10, 8, 6, 4, 4, 6, 8, 10,
};
std::vector<int> piece_evaluation_board_12{
        10, 8, 6, 4, 4, 6, 8, 10,
        8, 6, 4, 2, 2, 4, 6, 8,
        6, 4, 2, 0, 0, 2, 4, 6,
        4, 2, 0, -2, -2, 0, 2, 4,
        2, 0, -2, -4, -4, -2, 0, 2,
        0, -2, -4, -6, -6, -4, 0, 0,
        -2, -4, -6, -8, -8, -6, -4, -2,
        -4, -6, -8, -10, -10, -8, -6, -4,
};

int all_piece_evaluation[64 * 13]{

    //0
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        //1
         0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0,0,0,
        -5,-10,-10,-5,-5,-10,-10,-5,
        -10,-5, 0,16,16, 0,-5,-10,
        -5, 0,12, 8, 8, 12,0,-5,
         0, 10, 6, 0, 0, 6, 10,0,
         10, 5, 0, -5, -5, 0, 5, 10,
         0, 0, 0, 0, 0, 0, 0, 0,

         //2
         0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        //3
        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 8, 8, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,

        //4
        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,


        //5
        -5, -3, 0, 0, 0, 0, -3, -5,
        -3, 0, 0, 0, 0, 0, 0, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        -3, 0, 0, 0, 0, 0, 0, -3,
        -5, -3, 0, 0, 0, 0, -3, -5,


        //6
        -4, -6, -8, -10, -10, -8, -6, -4,
        -2, -4, -6, -8, -8, -6, -4, -2,
        0, -2, -4, -6, -6, -4, 0, 0,
        2, 0, -2, -4, -4, -2, 0, 2,
        4, 2, 0, -2, -2, 0, 2, 4,
        6, 4, 2, 0, 0, 2, 4, 6,
        8, 6, 4, 2, 2, 4, 6, 8,
        10, 8, 6, 4, 4, 6, 8, 10,


        //7
    10, 5, 0,-5,-5, 0, 5,10,
      0,10, 6, 0, 0, 6,10, 0,
     -5, 0,12, 8, 8,12, 0,-5,
    -10,-5, 0,16,16, 0,-5,-10,
     -5,-10,-10,-5,-5,-10,-10,-5,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,


      //8
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,


        //9
        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 8, 8, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,


        //10
       -15, -10, -10, -10, -10, -10, -10, -15,
       -10, -1, 0, 0, 0, 0, -1, -10,
       -10, 0, 3, 5, 5, 3, 0, -10,
       -10, 0, 5, 10, 10, 5, 0, -10,
       -10, 0, 5, 10, 10, 5, 0, -10,
       -10, 0, 3, 5, 5, 3, 0, -10,
       -10, -1, 0, 0, 0, 0, -1, -10,
       -15, -10, -10, -10, -10, -10, -10, -15,


       //11
      -5, -3, 0, 0, 0, 0, -3, -5,
      -3, 0, 0, 0, 0, 0, 0, -3,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      -3, 0, 0, 0, 0, 0, 0, -3,
      -5, -3, 0, 0, 0, 0, -3, -5,


      //12
      10, 8, 6, 4, 4, 6, 8, 10,
      8, 6, 4, 2, 2, 4, 6, 8,
      6, 4, 2, 0, 0, 2, 4, 6,
      4, 2, 0, -2, -2, 0, 2, 4,
      2, 0, -2, -4, -4, -2, 0, 2,
      0, -2, -4, -6, -6, -4, 0, 0,
      -2, -4, -6, -8, -8, -6, -4, -2,
      -4, -6, -8, -10, -10, -8, -6, -4,

};

int all_piece_evaluation_endgame[64 * 13] = {
    //0
            0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        //1

         100, 100, 100, 100, 100, 100, 100, 100,
         50, 50, 50, 50, 50, 50,50,50,
         30, 30, 30, 30, 30, 30,30,30,
         10, 10, 10, 10, 10, 10,10,10,
         5, 5, 5, 5, 5, 5,5,5,
         1, 1, 1, 1, 1, 1,1,1,
         0, 0, 0, 0, 0, 0,0,0,
         0, 0, 0, 0, 0, 0, 0, 0,

         //2

         0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        //3

        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 8, 8, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,

        //4

        -15, -10, -10, -10, -10, -10, -10, -15,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 3, 5, 5, 3, 0, -10,
        -10, -1, 0, 0, 0, 0, -1, -10,
        -15, -10, -10, -10, -10, -10, -10, -15,

        //5

        -5, -3, 0, 0, 0, 0, -3, -5,
        -3, 0, 0, 0, 0, 0, 0, -3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        -3, 0, 0, 0, 0, 0, 0, -3,
        -5, -3, 0, 0, 0, 0, -3, -5,

        //6

        -200, -100, -50, -30, -30, -50, -100, -200,
        -100, -50, -30, -10, -10, -30, -50, -100,
        -50, -30, 3, 5, 5, 3, -30, -50,
        -30, -10, 5, 10, 10, 5, -10, -30,
        -30, -10, 5, 10, 10, 5, -10, -30,
        -50, -30, 3, 5, 5, 3, -30, -50,
        -100, -50, -30, -10, -10, -30, -50, -100,
        -200, -100, -50, -30, -30, -50, -100, -200,

        //7

        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1,
        5, 5, 5, 5, 5, 5, 5, 5,
        10, 10, 10, 10, 10, 10, 10, 10,
        30, 30, 30, 30, 30, 30, 30, 30,
        50, 50, 50, 50, 50, 50, 50, 50,
        100, 100, 100, 100, 100, 100, 100, 100,


        //8

            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,

            //9

                -15, -10, -10, -10, -10, -10, -10, -15,
                -10, -1, 0, 0, 0, 0, -1, -10,
                -10, 0, 3, 5, 5, 3, 0, -10,
                -10, 0, 5, 10, 10, 5, 0, -10,
                -10, 0, 5, 8, 8, 5, 0, -10,
                -10, 0, 3, 5, 5, 3, 0, -10,
                -10, -1, 0, 0, 0, 0, -1, -10,
                -15, -10, -10, -10, -10, -10, -10, -15,

                //10

            -15, -10, -10, -10, -10, -10, -10, -15,
            -10, -1, 0, 0, 0, 0, -1, -10,
            -10, 0, 3, 5, 5, 3, 0, -10,
            -10, 0, 5, 10, 10, 5, 0, -10,
            -10, 0, 5, 10, 10, 5, 0, -10,
            -10, 0, 3, 5, 5, 3, 0, -10,
            -10, -1, 0, 0, 0, 0, -1, -10,
            -15, -10, -10, -10, -10, -10, -10, -15,

            //11

       -5, -3, 0, 0, 0, 0, -3, -5,
       -3, 0, 0, 0, 0, 0, 0, -3,
       0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0,
       -3, 0, 0, 0, 0, 0, 0, -3,
       -5, -3, 0, 0, 0, 0, -3, -5,

       //12

       -15, -10, -10, -10, -10, -10, -10, -15,
       -10, -1, 0, 0, 0, 0, -1, -10,
       -10, 0, 3, 5, 5, 3, 0, -10,
       -10, 0, 5, 10, 10, 5, 0, -10,
       -10, 0, 5, 10, 10, 5, 0, -10,
       -10, 0, 3, 5, 5, 3, 0, -10,
       -10, -1, 0, 0, 0, 0, -1, -10,
       -15, -10, -10, -10, -10, -10, -10, -15
};

int all_piece_values[13] = { 0,100,500,300,300,900,0,100,500,300,300,900,0 };



void calculateEndgameState(int board[], int turn) {
    endgame_state = 1;
    for (int i = 0;i < 64;++i) {
        if (board[i] != 0) {

            if (board[i] < 7) {
                if (turn == 3) {
                    endgame_state -= static_cast<float>(all_piece_values[board[i]]) / 3900;
                }
            }
            else {
                if (turn == 2) {
                    endgame_state -= static_cast<float> (all_piece_values[board[i]]) / 3900;
                }
            }
        }
    }
    //std::cout << endgame_state << std::endl;
}


int* black_and_white_pieces_into_list(int board[])
{
    //std::vector<int>A = board;
    int R[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, };
    for (int i = 0; i < (64); i++) {
        if (board[i] == 0) {
            R[i] = 1;
        }
        else if (board[i] > 6) {
            R[i] = 3;
        }
        else {
            R[i] = 2;
        }


    }
    //std::cout << R << std::endl;
    return R;
}

std::vector<int> queen_move(int n, int board[], int color_board[])
{
    int  opponent_color = 3 - color_board[n] % 2;
    std::vector<int> captures = {};
    for (int i = 1;i < 8 - n % 8;++i) {
        if (color_board[n + i] == 1) {
            captures.push_back(n + i);
        }
        else if (color_board[n + i] == opponent_color) {
            captures.push_back(n + i);
            break;
        }
        else if (color_board[n + i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < n % 8 + 1;++i) {
        if (color_board[n - i] == 1) {
            captures.push_back(n - i);
        }
        else if (color_board[n - i] == opponent_color) {
            captures.push_back(n - i);
            break;
        }
        else if (color_board[n - i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < 8 - n / 8;++i) {
        if (color_board[n + i * 8] == 1) {
            captures.push_back(n + i * 8);
        }
        else if (color_board[n + i * 8] == opponent_color) {
            captures.push_back(n + i * 8);
            break;
        }
        else if (color_board[n + i * 8] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < n / 8 + 1;++i) {
        if (color_board[n - 8 * i] == 1) {
            captures.push_back(n - 8 * i);
        }
        else if (color_board[n - 8 * i] == opponent_color) {
            captures.push_back(n - 8 * i);
            break;
        }
        else if (color_board[n - 8 * i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(8 - n % 8, 8 - n / 8);++i) {
        if (color_board[n + i * 9] == 1) {
            captures.push_back(n + i * 9);
        }
        else if (color_board[n + i * 9] == opponent_color) {
            captures.push_back(n + i * 9);
            break;
        }
        else if (color_board[n + i * 9] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(n % 8 + 1, n / 8 + 1);++i) {
        if (color_board[n + i * -9] == 1) {
            captures.push_back(n + i * -9);
        }
        else if (color_board[n + i * -9] == opponent_color) {
            captures.push_back(n + i * -9);
            break;
        }
        else if (color_board[n + i * -9] == color_board[n]) {
            break;
        }

    }

    for (int i = 1;i < min(n % 8 + 1, 8 - n / 8);++i) {
        if (color_board[n + i * 7] == 1) {
            captures.push_back(n + i * 7);
        }
        else if (color_board[n + i * 7] == opponent_color) {
            captures.push_back(n + i * 7);
            break;
        }
        else if (color_board[n + i * 7] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(8 - n % 8, n / 8 + 1);++i) {
        if (color_board[n + i * -7] == 1) {
            captures.push_back(n + i * -7);
        }
        else if (color_board[n + i * -7] == opponent_color) {
            captures.push_back(n + i * -7);
            break;
        }
        else if (color_board[n + i * -7] == color_board[n]) {
            break;
        }
    }
    return captures;
}
//}



std::vector<int> rook_move(int n, int board[], int color_board[])
{
    int  opponent_color = 3 - color_board[n] % 2;
    std::vector<int> captures = {};
    for (int i = 1;i < 8 - n % 8;++i) {
        if (color_board[n + i] == 1) {
            captures.push_back(n + i);
        }
        else if (color_board[n + i] == opponent_color) {
            captures.push_back(n + i);
            break;
        }
        else if (color_board[n + i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < n % 8 + 1;++i) {
        if (color_board[n - i] == 1) {
            captures.push_back(n - i);
        }
        else if (color_board[n - i] == opponent_color) {
            captures.push_back(n - i);
            break;
        }
        else if (color_board[n - i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < 8 - n / 8;++i) {
        if (color_board[n + i * 8] == 1) {
            captures.push_back(n + i * 8);
        }
        else if (color_board[n + i * 8] == opponent_color) {
            captures.push_back(n + i * 8);
            break;
        }
        else if (color_board[n + i * 8] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < n / 8 + 1;++i) {
        if (color_board[n - 8 * i] == 1) {
            captures.push_back(n - 8 * i);
        }
        else if (color_board[n - 8 * i] == opponent_color) {
            captures.push_back(n - 8 * i);
            break;
        }
        else if (color_board[n - 8 * i] == color_board[n]) {
            break;
        }
    }
    return captures;
}



std::vector<int> bishop_move(int n, int board[], int color_board[])
{
    /*
    std::vector<int> possible_moves = {};

    //1
    std::vector<int>diagonal = straigth_paths(n, -1, -1, board);
    for (int num : diagonal) {
        possible_moves.push_back(num);
    }
    //2
    diagonal = straigth_paths(n, -1, 1, board);
    for (int num : diagonal) {
        possible_moves.push_back(num);
    }
    //3
    diagonal = straigth_paths(n, 1, 1, board);
    for (int num : diagonal) {
        possible_moves.push_back(num);
    }
    //4
    diagonal = straigth_paths(n, 1, -1, board);
    for (int num : diagonal) {
        possible_moves.push_back(num);
    }
    return possible_moves;
}*/
//std::vector<int> color_board = black_and_white_pieces_into_list(board);
    int  opponent_color = 3 - color_board[n] % 2;
    std::vector<int> captures = {};
    for (int i = 1;i < min(8 - n % 8, 8 - n / 8);++i) {
        if (color_board[n + i * 9] == 1) {
            captures.push_back(n + i * 9);
        }
        else if (color_board[n + i * 9] == opponent_color) {
            captures.push_back(n + i * 9);
            break;
        }
        else if (color_board[n + i * 9] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(n % 8 + 1, n / 8 + 1);++i) {
        if (color_board[n + i * -9] == 1) {
            captures.push_back(n + i * -9);
        }
        else if (color_board[n + i * -9] == opponent_color) {
            captures.push_back(n + i * -9);
            break;
        }
        else if (color_board[n + i * -9] == color_board[n]) {
            break;
        }

    }

    for (int i = 1;i < min(n % 8 + 1, 8 - n / 8);++i) {
        if (color_board[n + i * 7] == 1) {
            captures.push_back(n + i * 7);
        }
        else if (color_board[n + i * 7] == opponent_color) {
            captures.push_back(n + i * 7);
            break;
        }
        else if (color_board[n + i * 7] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(8 - n % 8, n / 8 + 1);++i) {
        if (color_board[n + i * -7] == 1) {
            captures.push_back(n + i * -7);
        }
        else if (color_board[n + i * -7] == opponent_color) {
            captures.push_back(n + i * -7);
            break;
        }
        else if (color_board[n + i * -7] == color_board[n]) {
            break;
        }
    }
    return captures;
}

std::vector<int> knight_move(int n, int board[], int color_board[]) {
    int  opponent_color = 3 - color_board[n] % 2;
    int possible_captures[] = { 10,6,17,15,-6,-10,-17,-15 };
    std::vector<int> captures = {};
    //int vec;
    for (int kvec : possible_captures) {
        int vec = kvec + n;
        if (abs(n % 8 - vec % 8) < 3 && vec < 64 && vec >= 0 && color_board[vec] != color_board[n]) {
            captures.push_back(vec);
        }
    }
    return captures;
}

std::vector<int> king_move(int n, int board[], int color_board[]) {
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    std::vector<int> possible_moves = { n + 1,n + 9,n + 8,n + 7,n - 1,n - 9,n - 8,n - 7 };
    int start_x = n % 8;
    std::vector<int> checks = {};
    if (turn == 2 && board[66] == 0) {
        //std::cout << "????????????" << std::endl;
        if (board[67] == 0 && board[n - 1] == 0 && board[n - 2] == 0 && board[n - 3] == 0) {
            int wqscr = 0;
            for (int k : checks) {
                if (k == n - 2 || k == n - 1 || k == n) {
                    //std::cout << "CHECK" << std::endl;
                    wqscr = 1;
                }
            }
            if (wqscr == 0 && board[66] == 0 && board[67] == 0) {
                possible_moves.push_back(n - 2);
            }
        }
        if (board[68] == 0 && board[n + 1] == 0 && board[n + 2] == 0) {
            int wkscr = 0;
            for (int k : checks) {
                if (k == n + 2 || k == n + 1 || k == n) {
                    wkscr = 1;
                }
            }
            if (wkscr == 0 && board[66] == 0 && board[68] == 0) {
                possible_moves.push_back(n + 2);
            }
        }
    }
    else if (turn == 3 && board[69] == 0) {
        if (board[70] == 0 && board[n - 1] == 0 && board[n - 2] == 0 && board[n - 3] == 0) {
            int bqscr = 0;
            for (int k : checks) {
                if (k == n - 2 || k == n - 1 || k == n) {
                    bqscr = 1;
                }
            }
            if (bqscr == 0 && board[69] == 0 && board[70] == 0) {
                possible_moves.push_back(n - 2);
            }
        }
        if (board[71] == 0 && board[n + 1] == 0 && board[n + 2] == 0) {
            int bkscr = 0;
            for (int k : checks) {
                if (k == n + 2 || k == n + 1 || k == n) {
                    bkscr = 1;
                }
            }
            if (bkscr == 0 && board[69] == 0 && board[71] == 0) {
                possible_moves.push_back(n + 2);
            }
        }
    }

    for (int i = static_cast<int>(possible_moves.size()) - 1;i >= 0;--i) {
        if ((possible_moves[i] > 63 || possible_moves[i] < 0 || color_board[possible_moves[i]] == color_board[n] || abs(start_x - possible_moves[i] % 8)>2)) {
            possible_moves.erase(possible_moves.begin() + i);
        }
    }
    //std::cout << possible_moves.size() << std::endl;
    return possible_moves;
}

std::vector<int> pawn_move(int n, int board[], int color_board[])
{
    int en_passant_test = 0;
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    std::vector<int> possible_moves = {};
    int opo_color;
    int d;
    if (color_board[n] == 2) {
        opo_color = 3;
        d = -1;

    }
    else {
        opo_color = 2;
        d = 1;
    }
    if (n + 8 * d<64 && n + 8 * d>-1) {
        //if (care_about_checks == 0) {
        if (color_board[n + 8 * d] == 1) {
            possible_moves.push_back(n + 8 * d);
            if (n + 16 * d < 63 && 0 < n + 16 * d) {
                if (color_board[n + 16 * d] == 1 && (floor(n / 8) == 1 || floor(n / 8) == 6)) {
                    possible_moves.push_back(n + 16 * d);
                }
            }
        }
        if (color_board[n + 8 * d + 1] == opo_color && abs(n % 8 - (n + 8 * d + 1) % 8) < 3) {
            en_passant_test = 1;
            possible_moves.push_back(n + 8 * d + 1);
        }
        if (color_board[n + 8 * d - 1] == opo_color && abs(n % 8 - (n + 8 * d - 1) % 8) < 3) {
            en_passant_test = -1;
            possible_moves.push_back(n + 8 * d - 1);
        }
        //}
        //else {
        //    possible_moves.push_back(n + 8 * d + 1);
        //    possible_moves.push_back(n + 8 * d - 1);
        //}
    }
    if ((board[board[65]] == 1 || board[board[65]] == 7) && (abs(board[65] - board[64]) == 16)) {
        if (abs(n - board[65]) == 1) {
            possible_moves.push_back(n + 8 * d - (n - board[65]));
        }
    }
    return possible_moves;
}

std::vector<int> king_check_move(int n, int board[]) {
    std::vector<int> check_moves = { n + 1,n + 9,n + 8,n + 7,n - 1,n - 9,n - 8,n - 7 };
    return check_moves;
}

std::vector<int> pawn_check_move(int n, int board[], int color_board[]) {
    int d;
    if (board[n] == 7) {
        d = 1;
    }
    else {
        d = -1;
    }
    std::vector<int> check_moves = { n + 1 + 8 * d,n - 1 + 8 * d };
    return check_moves;
}

//using FunctionType2 = std::function<std::vector<int>(int, std::vector<int>&, std::vector<int>&)>;
//std::vector<FunctionType2> check_move_functions = { king_check_move,pawn_check_move,rook_move,knight_move,bishop_move,queen_move };

/*std::vector<int> check_moves(std::vector<int>& board, std::vector<int> color_board, int turn, int king_index) {
    std::vector<int> all_checks;
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    std::vector<int> possible_moves;
    for (int i = 0;i < 64;++i) {
        if (color_board[i] == turn) {
            if (((board[i] % 6 == 5 || board[i] % 6 == 4 || board[i] % 6 == 2) && ((abs(king_index - i) < 8) || ((king_index - i) % 8 == 0) || (king_index - i) % 7 == 0 || (king_index - i) % 9 == 0)) || ((abs(king_index - i) < 18) && ((board[i] % 6 == 1) || (board[i] % 6 == 0) || (board[i] % 6 == 3)))) {
                possible_moves = check_move_functions[board[i] % 6](i, board,color_board);
                for (int mov : possible_moves) {
                    all_checks.push_back(mov);
                    if (king_index == mov) {
                        return all_checks;
                    }
                }
            }
            possible_moves = {};
        }
    }
    return all_checks;
}*/

using FunctionType = std::function<std::vector<int>(int, int*, int*)>;
std::vector<FunctionType> move_functions = { king_move,pawn_move,rook_move,knight_move,bishop_move,queen_move };

bool checking_need_for_deletion(int board[], int color_board[], int king_index, int n, int turn) {
    int piece = board[n];
    int piece_color = color_board[n];
    board[n] = 0;
    color_board[n] = 1;
    for (int i = 1;i < 8 - king_index % 8;++i) {
        if (color_board[king_index + i] == turn) {
            break;
        }
        else if (color_board[king_index + i] == 3 - turn % 2) {
            if (board[king_index + i] % 6 == 2 || board[king_index + i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "s1" << std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }
    for (int i = 1;i < king_index % 8 + 1;++i) {
        if (color_board[king_index - i] == turn) {
            break;
        }
        else if (color_board[king_index - i] == 3 - turn % 2) {
            if (board[king_index - i] % 6 == 2 || board[king_index - i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "s2" << std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }
    for (int i = 1;i < 8 - king_index / 8;++i) {
        if (color_board[king_index + i * 8] == turn) {
            break;
        }
        else if (color_board[king_index + i * 8] == 3 - turn % 2) {
            if (board[king_index + 8 * i] % 6 == 2 || board[king_index + 8 * i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "s3" << std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }
    for (int i = 1;i < king_index / 8 + 1;++i) {
        if (color_board[king_index - i * 8] == turn) {
            break;
        }
        else if (color_board[king_index - i * 8] == 3 - turn % 2) {
            if (board[king_index - 8 * i] % 6 == 2 || board[king_index - 8 * i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "s4 "<< std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }
    //diagonals
    for (int i = 1;i < min(8 - king_index % 8, 8 - king_index / 8);++i) {
        if (color_board[king_index + i * 9] == turn) {
            break;
        }
        else if (color_board[king_index + i * 9] == 3 - turn % 2) {
            if (board[king_index + 9 * i] % 6 == 4 || board[king_index + 9 * i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "d1" << std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }
    for (int i = 1;i < min(king_index % 8 + 1, king_index / 8 + 1);++i) {
        if (color_board[king_index + i * -9] == turn) {
            break;
        }
        else if (color_board[king_index + i * -9] == 3 - turn % 2) {
            if (board[king_index - 9 * i] % 6 == 4 || board[king_index - 9 * i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "d2" << std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }

    for (int i = 1;i < min(king_index % 8 + 1, 8 - king_index / 8);++i) {
        if (color_board[king_index + i * 7] == turn) {
            break;
        }
        else if (color_board[king_index + i * 7] == 3 - turn % 2) {
            if (board[king_index + 7 * i] % 6 == 4 || board[king_index + 7 * i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "d3" << std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }
    for (int i = 1;i < min(8 - king_index % 8, king_index / 8 + 1);++i) {
        if (color_board[king_index + i * -7] == turn) {
            break;
        }
        else if (color_board[king_index + i * -7] == 3 - turn % 2) {
            if (board[king_index - 7 * i] % 6 == 4 || board[king_index - 7 * i] % 6 == 5) {
                board[n] = piece;
                color_board[n] = piece_color;
                //std::cout << "d4" << std::endl;
                return false;
            }
            else {
                break;
            }
        }
    }
    //std::vector<int> horse_positions= { king_index + 10,king_index + 6,king_index + 17,king_index + 15,king_index - 6,king_index - 10,king_index - 17,king_index - 15 };
    const int horse_positions[] = { 10, 6, 17, 15, -6, -10, -17, -15 };

    for (int hmov : horse_positions) {
        int mov = king_index + hmov;
        if (mov<64 && mov>-1) {
            if (abs(king_index % 8 - mov % 8) < 3) {
                if (color_board[mov] == 3 - turn % 2 && board[mov] % 6 == 3) {
                    board[n] = piece;
                    color_board[n] = piece_color;
                    //std::cout << "horse" << std::endl;
                    return false;
                }
            }
        }
    }
    //std::vector<int> king_positions = { king_index + 1,king_index + 9,king_index + 8,king_index + 7,king_index - 1,king_index - 9,king_index - 8,king_index -7 };
    const int king_positions[] = { 1, 9, 8, 7, -1, -9, -8, -7 };
    for (int kmov : king_positions) {
        int mov = king_index + kmov;
        if (mov<64 && mov>-1) {
            if (abs(king_index % 8 - mov % 8) < 3) {
                if (color_board[mov] == 3 - turn % 2 && board[mov] % 6 == 0) {
                    board[n] = piece;
                    color_board[n] = piece_color;
                    //std::cout << "king" << std::endl;
                    return false;
                }
            }
        }
    }

    int d;
    if (turn == 3) {
        d = 1;
    }
    else {
        d = -1;
    }
    //std::cout << "d " << d <<" "<< king_index + 1 + 8 * d << std::endl;
    if (king_index % 8 < 7 && king_index / 8 + d < 8 && king_index / 8 + d >0) {
        if (board[king_index + 1 + 8 * d] == 1 + (1 - d) * 3) {
            board[n] = piece;
            color_board[n] = piece_color;
            //std::cout << "pawn" << std::endl;
            return false;
        }

    }

    if (king_index % 8 > 0 && king_index / 8 + d < 8 && king_index / 8 + d >0) {
        if (board[king_index - 1 + 8 * d] == 1 + (1 - d) * 3) {
            board[n] = piece;
            color_board[n] = piece_color;
            //std::cout << "pawn" << std::endl;
            return false;
        }
    }

    board[n] = piece;
    color_board[n] = piece_color;
    //std::cout << ">: "<<king_index << std::endl;
    return true;
}

std::vector<int> move(int n, int board[72], int color_board[72], int care_about_checks, int turn) {
    //std::cout << "d1" << std::endl;

    std::vector<int> possible_moves = move_functions[board[n] % 6](n, board, color_board);
    //std::cout << "d2" << std::endl;

    if (care_about_checks == 0) {
        int king_index = -1;
        if (turn == 2) {
            for (int i = 0;i < 64;++i) {
                if (board[i] == 6) {
                    king_index = i;
                    break;
                }
            }
        }
        else {
            for (int i = 0;i < 64;++i) {
                if (board[i] == 12) {
                    king_index = i;
                    break;
                }
            }
        }
        if (false == checking_need_for_deletion(board, color_board, king_index, n, turn) || board[n] % 6 == 0) {
            int saved_board[72];
            for (int i = 0; i < 72; ++i) {
                saved_board[i] = board[i];
            }
            const int move_size = possible_moves.size();
            int u = 0;
            for (int mov = 0;mov < move_size;++mov) {
                make_move(board, n, possible_moves[mov - u]);
                color_board = black_and_white_pieces_into_list(board);
                if (board[possible_moves[mov - u]] % 6 == 0) {
                    if (turn == 2) {
                        for (int i = 0;i < 64;++i) {
                            if (board[i] == 6) {
                                king_index = i;
                                break;
                            }
                        }
                    }
                    else {
                        for (int i = 0;i < 64;++i) {
                            if (board[i] == 12) {
                                king_index = i;
                                break;
                            }
                        }
                    }
                }
                if (false == checking_need_for_deletion(board, color_board, king_index, n, turn)) {
                    possible_moves.erase(possible_moves.begin() + (mov - u));
                    ++u;

                }

                for (int i = 0; i < 72; ++i) {
                    board[i] = saved_board[i];
                }
            }
        }
        //}
    }

    //possible_moves.insert(possible_moves.begin(), n);
    return possible_moves;
}

void DrawChessboard(HDC hdc, int boardSize, int squareSize) {
    std::cout << calculateBoardHash(visible_board, turn) << std::endl;
    // Set initial position
    int x = 0;
    int y = 0;

    const wchar_t* chessPieces[] = {
        L"\u25A1", L"\u265F", L"\u265C", L"\u265E", L"\u265D", L"\u265B", L"\u265A",
        L"\u2659", L"\u2656", L"\u2658", L"\u2657", L"\u2655", L"\u2654"
    };

    // Loop through rows and columns to draw the chessboard
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            // Set the color based on the position
            COLORREF color = RGB(0, 0, 0);
            if (visible_board[64] == row * boardSize + col || visible_board[65] == row * boardSize + col) {
                color = (row + col) % 2 == 0 ? RGB(0, 0, 255) : RGB(0, 0, 255);
            }
            else {
                color = (row + col) % 2 == 0 ? RGB(255, 255, 255) : RGB(0, 0, 0);
            }
            // Create and fill the rectangle
            RECT squareRect = { x, y, x + squareSize, y + squareSize };
            HBRUSH hBrush = CreateSolidBrush(color);
            FillRect(hdc, &squareRect, hBrush);
            DeleteObject(hBrush);
            int piece = visible_board[(row * boardSize + col)];
            if (piece != 0) {
                // Adjust the text color and size as needed
                SetTextColor(hdc, RGB(255, 0, 0));  // Red color for the piece
                SetBkMode(hdc, TRANSPARENT);  // Set text background mode to transparent

                std::wstringstream wss;
                wss << chessPieces[piece];
                std::wstring pieceStr = wss.str();
                int color_board[72];
                int* result = black_and_white_pieces_into_list(visible_board);

                for (int i = 0; i < 72; ++i) {
                    color_board[i] = result[i];
                }

                if (color_board[row * boardSize + col] == 2) {
                    SetTextColor(hdc, RGB(255, 0, 0));
                }
                else {
                    SetTextColor(hdc, RGB(0, 255, 0));
                }

                LOGFONT lf;
                GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(LOGFONT), &lf);
                lf.lfHeight = 24 * 2; // Set the font size here
                lf.lfWidth = 18 * 2;
                HFONT hFont = CreateFontIndirect(&lf);
                SelectObject(hdc, hFont);

                TextOutW(hdc, x + squareSize / 2 - 18, y + squareSize / 2, pieceStr.c_str(), static_cast<int>(pieceStr.size()));
            }

            // Move to the next column
            x += squareSize;
        }

        // Move to the next row and reset the column position
        x = 0;
        y += squareSize;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw the chessboard
        DrawChessboard(hdc, 8, 50);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        //std::cout << "h" << std::endl;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int clickedCol = xPos / 50;  // Assuming squareSize is 50
        int clickedRow = yPos / 50;

        // Handle left mouse button click at (xPos, yPos)
        // You can add your logic here
        int clickedPieceIndex = clickedRow * 8 + clickedCol;


        // Handle piece selection
        if (selectedPieceIndex == -1) {
            //std::cout << "square clicked " << clickedPieceIndex << std::endl;
            int* color_board = black_and_white_pieces_into_list(visible_board);
            // If no piece is selected, check if there's a piece on the clicked square
            if (visible_board[clickedPieceIndex] != 0 && color_board[clickedPieceIndex] == turn) {
                // Set the selected piece index
                selectedPieceIndex = clickedPieceIndex;
            }
        }
        else {
            int* color_board = black_and_white_pieces_into_list(visible_board);
            std::vector<int> k = move(selectedPieceIndex, visible_board, color_board, 0, turn);
            for (int i = 0; i < static_cast<int>(k.size());i++) {
                if (k[i] == clickedPieceIndex) {
                    make_move(visible_board, selectedPieceIndex, clickedPieceIndex);
                    selectedPieceIndex = -1;
                    turn = 3 - turn % 2;
                    InvalidateRect(hwnd, nullptr, TRUE);
                }


            }
            selectedPieceIndex = -1;
        }
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        if (turn == 3 && computer == 0) {
            performAgeBasedDeletion(transpositionTable, 4);
            std::cout << transpositionTable.size() << std::endl;
            auto start_time = std::chrono::steady_clock::now();
            auto end_time = std::chrono::steady_clock::now();
            int mdepth = 0;
            std::tuple<int, int> move_to_make = std::make_tuple(-1, -1);
            moves_made = 0;
            // while (true) {
            //while (mdepth < 6) {
            while (std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() < 2) {
                //start_time = std::chrono::steady_clock::now();
                mdepth += 1;
                move_to_make = finding_best_move(visible_board, turn, mdepth, move_to_make);
                end_time = std::chrono::steady_clock::now();
                std::cout << "depth " << mdepth << " time " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " moves " << moves_made<<" found lookups "<<found_lookup << std::endl;
                //std::cout << std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() << std::endl;
                found_lookup = 0;
                moves_made = 0;
            //}
            }
            //std::cout << "next move" << std::endl;
            //end_time = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " " << mdepth << " NPS " << (moves_made * 1000) / (std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()) << std::endl;
            //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << std::endl;
            std::cout << std::get<0>(move_to_make) << "  " << visible_board[std::get<0>(move_to_make)] << "  " << std::get<1>(move_to_make) << "  " << visible_board[std::get<1>(move_to_make)] << std::endl;
            make_move(visible_board, std::get<0>(move_to_make), std::get<1>(move_to_make));
            std::cout << visible_board[64] << "  " << visible_board[65] << std::endl;
            turn = 3 - turn % 2;
            //std::cout << "next move" << std::endl;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int evaluate(int board[]) {
    moves_made += 1;
    float evaluation = 0;
    //int blackKingIndex;
    //int whiteKingIndex;
    for (int i = 0;i < 64;i++) {
        if (board[i] == 0) {
            continue;
        }
        else if (board[i] < 7) {
            //if (board[i]==12){
            //    blackKingIndex = i;
            //}
           // evaluation += (static_cast<float>(all_piece_evaluation[64 * board[i] + i])*(1-endgame_state)+ static_cast<float>(all_piece_evaluation_endgame[64 * board[i] + i])*endgame_state);
            evaluation += ((all_piece_evaluation[64 * board[i] + i]) * (1 - endgame_state) + (all_piece_evaluation_endgame[64 * board[i] + i]) * endgame_state);
            evaluation += all_piece_values[board[i]];
        }
        else {
            //if (board[i] == 6) {
            //    whiteKingIndex = i;
            //}
            evaluation -= ((all_piece_evaluation[64 * board[i] + i]) * (1 - endgame_state) + (all_piece_evaluation_endgame[64 * board[i] + i]) * endgame_state);
            evaluation -= all_piece_values[board[i]];
        }
    }
    //evaluation -= (endgame_state) * 10 * (abs(whiteKingIndex % 8 - blackKingIndex % 8) + abs(whiteKingIndex / 8 - blackKingIndex / 8));
    return static_cast<int>(evaluation);
}

void make_move(int board[72], int selectedPieceIndex, int clickedPieceIndex) {
    //moves_made += 1;
    if ((board[board[65]] == 1 || board[board[65]] == 7) && (abs(board[65] - board[64]) == 16)) {
        if (abs(selectedPieceIndex - board[65]) == 1) {
            board[board[65]] = 0;
        }
    }

    if ((board[selectedPieceIndex] == 6 || board[selectedPieceIndex] == 12) && (abs(selectedPieceIndex - clickedPieceIndex) == 2)) {
        if (board[selectedPieceIndex] == 6) {
            if (selectedPieceIndex - clickedPieceIndex == 2) {
                board[56] = 0;
                board[59] = 2;
            }
            else if (selectedPieceIndex - clickedPieceIndex == -2) {
                board[63] = 0;
                board[61] = 2;
            }
        }
        if (board[selectedPieceIndex] == 12) {
            if (selectedPieceIndex - clickedPieceIndex == 2) {
                board[0] = 0;
                board[3] = 8;
            }
            if (selectedPieceIndex - clickedPieceIndex == -2) {
                board[7] = 0;
                board[5] = 8;
            }
        }
    }

    board[64] = selectedPieceIndex;
    board[65] = clickedPieceIndex;
    board[clickedPieceIndex] = board[selectedPieceIndex];
    board[selectedPieceIndex] = 0;

    if ((board[clickedPieceIndex] == 1 || board[clickedPieceIndex] == 7) &&
        ((clickedPieceIndex / 8 == 0) || clickedPieceIndex / 8 == 7)) {
        if (board[clickedPieceIndex] == 1) {
            board[clickedPieceIndex] = 5;
        }
        else {
            board[clickedPieceIndex] = 11;
        }
    }
    if (board[clickedPieceIndex] % 6 == 2 || board[clickedPieceIndex] % 6 == 0) {
        if (board[4] != 12) {
            board[69] = 1;
        }
        else if (board[0] != 8) {
            board[70] = 1;
        }
        else if (board[7] != 8) {
            board[71] = 1;
        }

        else if (board[60] != 6) {
            board[66] = 1;
        }
        else if (board[56] != 2) {
            board[67] = 1;
        }
        else if (board[63] != 2) {
            board[68] = 1;
        }
    }


    //return board;
}

std::vector<std::tuple<int, int, int>> creating_ordered_moves(int board[], int turn) {
    int move_importance = 0;
    int color_board[72];
    int* modified_board = black_and_white_pieces_into_list(board);
    for (int i = 0; i < 72; ++i) {
        color_board[i] = modified_board[i];
    }

    std::vector<std::tuple<int, int, int>> all_possible_moves = {};
    for (int i = 0; i < 64;i++) {
        if (turn == color_board[i]) {
            if (color_board[i] == 2) {
                if (board[i] > 7 || board[i] == 0) {
                    std::cout << turn << " " << color_board[i] << "        ERROR                       white " << board[i] << std::endl;
                }
            }
            else {
                if (board[i] < 7) {
                    for (int y = 0;y < 8;++y) {
                        for (int x = 0;x < 8;++x) {
                            std::cout << color_board[8 * y + x] << " ";
                        }
                        std::cout << "  " << std::endl;
                    }
                    std::cout << "  " << std::endl;
                    for (int y = 0;y < 8;++y) {
                        for (int x = 0;x < 8;++x) {
                            std::cout << board[8 * y + x] << " ";
                        }
                        std::cout << "  " << std::endl;
                    }
                    std::cout << "  " << std::endl;
                    std::cout << turn << " " << color_board[i] << "  ERROR                      black    " << board[i] << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(100));
                }
            }
            std::vector<int> possible_moves_for_a_piece = move(i, board, color_board, 0, turn);
            for (int j = 0;j < possible_moves_for_a_piece.size();j++) {
                move_importance += all_piece_evaluation[64 * board[i] + possible_moves_for_a_piece[j]] - all_piece_evaluation[64 * board[i] + i];
                if (board[possible_moves_for_a_piece[j]] != 0) {
                    move_importance += (10 * all_piece_values[board[possible_moves_for_a_piece[j]]]) - all_piece_values[board[i]];
                }
                all_possible_moves.push_back(std::make_tuple(move_importance, i, possible_moves_for_a_piece[j]));
                move_importance = 0;
            }
        }
    }
    std::sort(all_possible_moves.begin(), all_possible_moves.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
        });

    std::reverse(all_possible_moves.begin(), all_possible_moves.end());
    return all_possible_moves;
}

std::vector<int> king_capture(int n, int board[], int color_board[]) {
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    int  opponent_color = 3 - color_board[n];
    std::vector<int> possible_captures = { n + 1,n + 9,n + 8,n + 7,n - 1,n - 9,n - 8,n - 7 };
    std::vector<int> captures = {};
    for (int vec : possible_captures) {
        if (board[vec] == opponent_color) {
            if (abs(n % 8 - vec % 8) < 3 && vec < 64 && vec >= 0) {
                captures.push_back(vec);
            }
        }
    }
    return captures;
}

std::vector<int> pawn_capture(int n, int board[], int color_board[]) {
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    int  opponent_color = 3 - color_board[n] % 2;
    int d;
    if (board[n] < 7) {
        d = -1;
    }
    else {
        d = 1;
    }
    std::vector<int> possible_captures = { n + 8 * d + 1,n + 8 * d - 1 };
    std::vector<int> captures = {};
    for (int vec : possible_captures) {
        if (color_board[vec] == opponent_color) {
            captures.push_back(vec);
        }
    }
    return captures;
}

std::vector<int> knight_capture(int n, int board[], int color_board[]) {
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    int  opponent_color = 3 - color_board[n] % 2;
    std::vector<int> possible_captures = { n + 10,n + 6,n + 17,n + 15,n - 6,n - 10,n - 17,n - 15 };
    std::vector<int> captures = {};
    for (int vec : possible_captures) {
        if (color_board[vec] == opponent_color) {
            if (abs(n % 8 - vec % 8) < 3 && vec < 64 && vec >= 0) {
                captures.push_back(vec);
            }
        }
    }
    return captures;
}

std::vector<int> rook_capture(int n, int board[], int color_board[]) {
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    int  opponent_color = 3 - color_board[n] % 2;
    std::vector<int> captures = {};
    for (int i = 1;i < 8 - n % 8;++i) {
        if (color_board[n + i] == opponent_color) {
            captures.push_back(n + i);
            break;
        }
        else if (color_board[n + i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i <= n % 8;++i) {
        if (color_board[n - i] == opponent_color) {
            captures.push_back(n - i);
            break;
        }
        else if (color_board[n - i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < 8 - n / 8;++i) {
        if (color_board[n + i * 8] == opponent_color) {
            captures.push_back(n + i * 8);
            break;
        }
        else if (color_board[n + i * 8] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i <= n / 8;++i) {
        if (color_board[n - 8 * i] == opponent_color) {
            captures.push_back(n - 8 * i);
            break;
        }
        else if (color_board[n - 8 * i] == color_board[n]) {
            break;
        }
    }
    return captures;
}

std::vector<int> bishop_capture(int n, int board[], int color_board[]) {
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    int  opponent_color = 3 - color_board[n] % 2;
    std::vector<int> captures = {};
    for (int i = 1;i < min(8 - n % 8, 8 - n / 8);++i) {
        if (color_board[n + i * 9] == opponent_color) {
            captures.push_back(n + i * 9);
            break;
        }
        else if (color_board[n + i * 9] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(n % 8 + 1, n / 8 + 1);++i) {
        if (color_board[n + i * -9] == opponent_color) {
            captures.push_back(n + i * -9);
            break;
        }
        else if (color_board[n + i * -9] == color_board[n]) {
            break;
        }

    }

    for (int i = 1;i < min(n % 8 + 1, 8 - n / 8);++i) {
        if (color_board[n + i * 7] == opponent_color) {
            captures.push_back(n + i * 7);
            break;
        }
        else if (color_board[n + i * 7] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(8 - n % 8, n / 8 + 1);++i) {
        if (color_board[n + i * -7] == opponent_color) {
            captures.push_back(n + i * -7);
            break;
        }
        else if (color_board[n + i * -7] == color_board[n]) {
            break;
        }
    }
    return captures;
}

std::vector<int> queen_capture(int n, int board[], int color_board[]) {
    //std::vector<int> color_board = black_and_white_pieces_into_list(board);
    int  opponent_color = 3 - color_board[n] % 2;
    std::vector<int> captures = {};
    for (int i = 1;i < 8 - n % 8;++i) {
        if (color_board[n + i] == opponent_color) {
            captures.push_back(n + i);
            break;
        }
        else if (color_board[n + i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i <= n % 8;++i) {
        if (color_board[n - i] == opponent_color) {
            captures.push_back(n - i);
            break;
        }
        else if (color_board[n - i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < 8 - n / 8;++i) {
        if (color_board[n + i * 8] == opponent_color) {
            captures.push_back(n + i * 8);
            break;
        }
        else if (color_board[n + i * 8] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i <= n / 8;++i) {
        if (color_board[n - 8 * i] == opponent_color) {
            captures.push_back(n - 8 * i);
            break;
        }
        else if (color_board[n - 8 * i] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i < min(8 - n % 8, 8 - n / 8);++i) {
        if (color_board[n + i * 9] == opponent_color) {
            captures.push_back(n + i * 9);
            break;
        }
        else if (color_board[n + i * 9] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i <= min(n % 8 + 1, n / 8 + 1);++i) {
        if (color_board[n + i * -9] == opponent_color) {
            captures.push_back(n + i * -9);
            break;
        }
        else if (color_board[n + i * -9] == color_board[n]) {
            break;
        }

    }

    for (int i = 1;i < min(n % 8 + 1, 8 - n / 8);++i) {
        if (color_board[n + i * 7] == opponent_color) {
            captures.push_back(n + i * 7);
            break;
        }
        else if (color_board[n + i * 7] == color_board[n]) {
            break;
        }
    }
    for (int i = 1;i <= min(8 - n % 8, n / 8 + 1);++i) {
        if (color_board[n + i * -7] == opponent_color) {
            captures.push_back(n + i * -7);
            break;
        }
        else if (color_board[n + i * -7] == color_board[n]) {
            break;
        }
    }
    return captures;
}

//using FunctionType3 = std::function<std::vector<int>(int, std::vector<int>&, std::vector<int>&)>;
//std::vector<FunctionType3> capture_functions = { king_capture,pawn_capture,rook_capture,knight_capture,bishop_capture,queen_capture };
/*
std::vector<int> capture_move(int n, int board[], int color_board[], int turn){
    std::vector<int> possible_moves = capture_functions[board[n] % 6](n, board,color_board);
    int king_index = -1;
    if (turn == 2) {
        for (int i = 0;i < 64;++i) {
            if (board[i] == 6) {
                king_index = i;
                break;
            }
        }
    }
    else {
        for (int i = 0;i < 64;++i) {
            if (board[i] == 12) {
                king_index = i;
                break;
            }
        }
    }
    if (false == checking_need_for_deletion(board,color_board, king_index, n, turn) || board[n] % 6 == 0) {
        std::vector<int> saved_board = board;
        const int move_size = possible_moves.size();
        int u = 0;
        for (int mov = 0;mov < move_size;++mov) {
            board = make_move(board, n, possible_moves[mov - u]);
            if (board[possible_moves[mov - u]] % 6 == 0) {
                if (turn == 2) {
                    for (int i = 0;i < 64;++i) {
                        if (board[i] == 6) {
                            king_index = i;
                            break;
                        }
                    }
                }
                else {
                    for (int i = 0;i < 64;++i) {
                        if (board[i] == 12) {
                            king_index = i;
                            break;
                        }
                    }
                }
            }
            if (false == checking_need_for_deletion(board,color_board, king_index, n, turn)) {
                possible_moves.erase(possible_moves.begin() + (mov - u));
                ++u;

            }

            board = saved_board;
        }
    }
    possible_moves.insert(possible_moves.begin(), n);
    for (int vec : possible_moves) {
        if (vec>63||vec<0)
        std::cout << vec <<"     "<< board[n]<< '\n';
    }
    return possible_moves;
}


std::vector<std::tuple<int, int, int>> creating_ordered_captures(int board[], int turn) {

    int move_importance = 0;
    std::vector<int> color_board = black_and_white_pieces_into_list(board);
    std::vector<std::tuple<int, int, int>> all_possible_moves = {};
    for (int i = 0; i < 64;i++) {
        if (turn == color_board[i]) {
            std::vector<int> possible_moves_for_a_piece = capture_move(i, board, color_board, turn);
            for (int j = 1;j < possible_moves_for_a_piece.size();j++) {
                if (board[possible_moves_for_a_piece[j]] != 0) {
                    move_importance += (10 * all_piece_values[board[possible_moves_for_a_piece[j]]]) - all_piece_values[board[i]];
                    all_possible_moves.push_back(std::make_tuple(move_importance, i, possible_moves_for_a_piece[j]));
                }
                move_importance = 0;
            }
        }
    }
    std::sort(all_possible_moves.begin(), all_possible_moves.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
        });

    std::reverse(all_possible_moves.begin(), all_possible_moves.end());
    return all_possible_moves;
}

int only_captures_search(int board[], int turn, int depth, int alpha) {
    if (depth == 5){
        return evaluate(board);
    }
    std::vector<std::tuple<int, int, int>> all_possible_captures = creating_ordered_captures(board, turn);

    if (all_possible_captures.size() == 0){
        return evaluate(board);
    }
   // if
    //std::cout << all_possible_captures.size() << std::endl;
    std::vector<int> saved_board = board;
    int best_e;
    int e;
    if (turn == 2) {
        best_e = -10000000;
    }
    else {
        best_e = 100000000;
    }
    for (std::tuple<int, int, int> vec : all_possible_captures) {
        board = make_move(board, std::get<1>(vec), std::get<2>(vec));
        e = only_captures_search(board, 3 - turn % 2,depth+1,best_e);
        board = saved_board;
        if (turn == 2) {
            if (e >= alpha) {
                return e;
            }
            if (e > best_e) {
                best_e = e;
            }
        }
        else {
            if (e <= alpha) {
                return e;
            }
            if (e < best_e) {
                best_e = e;
            }
        }
    }
    return best_e;
}

*/

std::uint64_t xorshift(std::uint64_t state) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

std::uint64_t old_f(int square_index, int piece_index) {
    // Assuming 6 bits for square_index (0 to 63) and 4 bits for piece_index (0 to 12)
    return static_cast<std::uint64_t>(square_index) | (static_cast<std::uint64_t>(piece_index) << 6);
}

std::uint64_t f(int square_index, int piece_index) {
    // Assuming 6 bits for square_index (0 to 63) and 4 bits for piece_index (0 to 12)
    std::hash<int> intHasher;

    // Combine the hash values of square_index and piece_index
    std::uint64_t hash = static_cast<std::uint64_t>(intHasher(square_index));
    hash ^= static_cast<std::uint64_t>(intHasher(piece_index)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

    return hash;
}

void initializeZobristKeys() {
    std::uint64_t seed = 1125899906842597;  // Use a seed of your choice
   // const std::uint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    for (int i = 0; i < 64; ++i) {
        for (int piece = 0;piece < 13;piece++) {
            pieceKeys[old_f(i,piece)] = xorshift(seed + f(i, piece));
            //std::cout << "f " << f(i, piece)<<" "<< xorshift(seed + f(i, piece)) << std::endl;
        }
    }

    sideKey = xorshift(seed +675875);
}

std::uint64_t calculateBoardHash(int board[72], int turn) {
    std::uint64_t hash = 0;

    for (int i = 0; i < 64; ++i) {
        //int piece = board[index];
        //if (board[i] != 0) {
            hash ^= pieceKeys[old_f(i,board[i])];
        //}
    }
    if (turn == 2) {
        hash ^= sideKey;
    }

    return hash;
}

//void storeInTranspositionTable(std::uint64_t hash, int best_move_piece, int best_move_to, int depth, int evaluation) {
void storeInTranspositionTable(std::uint64_t hash, int depth, int evaluation) {
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end()) {
        // Entry found, update the information
        TranspositionTableEntry& entry = it->second;
        //entry.best_move_piece = best_move_piece;
        //entry.best_move_to = best_move_to;
        entry.depth = depth;
        entry.evaluation = evaluation;
        // Update other fields as needed
    }
    else {
        // Entry not found, create a new one
        //TranspositionTableEntry newEntry = { best_move_piece, best_move_to,depth, evaluation};
        TranspositionTableEntry newEntry = {hash,0,depth, evaluation };
        transpositionTable[hash] = newEntry;
    }
}

void performAgeBasedDeletion(std::unordered_map<std::uint64_t, TranspositionTableEntry>& table, int maxAge) {
   // std::cout<<table.size()<< std::endl;
    for (auto it = table.begin(); it != table.end();) {
        if (it->second.age > maxAge) {
           
            it = table.erase(it);
        }
        else {
            //std::cout << "f" << std::endl;
            ++(it->second.age);
            ++it;
        }
    }
    //std::cout << table.size() << std::endl;

}


int search(int board[], int turn, int depth, int starting_depth, int alpha, int aspiration_window_for_white, int aspiration_window_for_black) {
    int best_e;
    if (turn == 2) {
        best_e = -10000000;
    }
    else {
        best_e = 100000000;
    }
    if (depth == 0) {
        return evaluate(board);
    }

    std::uint64_t hash = calculateBoardHash(board, turn);
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end()) {
        if (it->second.depth >= depth) {
            //std::cout << depth- it->second.depth << '\n';
            found_lookup += 1;
            return it->second.evaluation;
        }
    }
    
    std::vector<std::tuple<int, int, int>> all_possible_moves = creating_ordered_moves(board, turn);
    if (all_possible_moves.size() == 0) {
        int color_board[72];
        int* modified_board = black_and_white_pieces_into_list(board);
        for (int i = 0; i < 72; ++i) {
            color_board[i] = modified_board[i];
        }
        int king_index = -1;
        if (turn == 2) {
            for (int i = 0;i < 64;++i) {
                if (board[i] == 6) {
                    king_index = i;
                    break;
                }
            }
        }
        else {
            for (int i = 0;i < 64;++i) {
                if (board[i] == 12) {
                    king_index = i;
                    break;
                }
            }
        }
        if (false == checking_need_for_deletion(board, color_board, king_index, king_index, turn)) {
            return best_e;
        }
        else {
            return 0;
        }
    }
    int saved_board[72];
    for (int i = 0; i < 72; ++i) {
        saved_board[i] = board[i];
    }
    //std::vector<int> evaluations = {};
    int e;

    for (std::tuple<int, int, int> vec : all_possible_moves) {
        make_move(board, std::get<1>(vec), std::get<2>(vec));
        /*if (turn == 2) {
            e = search(board, 3 - turn % 2, depth - 1, starting_depth, best_e, evaluate(board), aspiration_window_for_black);
        }
        else {
            e = search(board, 3 - turn % 2, depth - 1, starting_depth, best_e, aspiration_window_for_white, evaluate(board));
        }
        */
        e = search(board, 3 - turn % 2, depth - 1, starting_depth, best_e, 0, 0);
        for (int i = 0; i < 72; ++i) {
            board[i] = saved_board[i];
        }
        if (turn == 2) {
            if (e >= alpha) {
                return e;
            }
            if (e > best_e) {
                best_e = e;
            }

        }
        else {
            if (e <= alpha) {
                return e;
            }
            if (e < best_e) {
                best_e = e;
            }
        }
        //evaluations.push_back(e);


    }
    //std::cout << "h" << std::endl;
    storeInTranspositionTable(hash, depth, best_e);
    return best_e;
}

std::tuple<int, int> finding_best_move(int board[], int turn, int depth, std::tuple<int, int> previous_best_move) {
    calculateEndgameState(board, turn);
    //std::cout<<"endgame state " << endgame_state << std::endl;
    //std::cout << turn << " turn" << std::endl;
    std::vector<std::tuple<int, int, int>> all_possible_moves = creating_ordered_moves(board, turn);

    if (std::get<0>(previous_best_move) != -1) {
        all_possible_moves.insert(all_possible_moves.begin(), (std::make_tuple(0, std::get<0>(previous_best_move), std::get<1>(previous_best_move))));
    }

    int saved_board[72];
    for (int i = 0; i < 72; ++i) {
        saved_board[i] = board[i];
    }
    std::vector<int> evaluations = {};
    int best_e = 10000000000;
    std::tuple<int, int> move_to_make = std::make_tuple(0, 0);
    if (all_possible_moves.size() == 0) {
        std::cout << "game ended";
    }
    int e;
    for (std::tuple<int, int, int> vec : all_possible_moves)
    {
        make_move(board, std::get<1>(vec), std::get<2>(vec));
        std::uint64_t hash = calculateBoardHash(board, turn);
        auto it = transpositionTable.find(hash);
        if (it != transpositionTable.end()) {
            if (it->second.depth >= depth) {
                //std::cout << depth- it->second.depth << '\n';
                found_lookup += 1;
                //return it->second.evaluation;
            }
            else{
                e = search(board, 3 - turn % 2, depth - 1, depth, best_e, evaluate(board), evaluate(board));
            }
        }
        else{ 
            e = search(board, 3 - turn % 2, depth - 1, depth, best_e, evaluate(board), evaluate(board)); 
        }
        //e = search(board, 3 - turn % 2, depth - 1, depth, best_e, evaluate(board), evaluate(board));
        for (int i = 0; i < 72; ++i) {
            board[i] = saved_board[i];
        }
        if (e < best_e) {
            best_e = e;
            move_to_make = std::make_tuple(std::get<1>(vec), std::get<2>(vec));
        }
    }


    /*
    std::vector<int> saved_board = board;
    //std::vector<int> evaluations = {};
    int best_e = 10000000000;
    std::tuple<int, int> move_to_make = std::make_tuple(0, 0);
    if (all_possible_moves.size() == 0) {
        std::cout << "game ended";
    }
    int e;
#pragma omp parallel
    {
#pragma omp for
        for (int i=0;i < all_possible_moves.size();++i)
        {
            std::vector<int> pboard = board;
            std::vector<int> psaved_board = pboard;
            std::tuple<int, int, int> vec = all_possible_moves[i];
            pboard = make_move(pboard, std::get<1>(vec), std::get<2>(vec));
            e = search(pboard, 3 - turn % 2, depth - 1, depth, best_e);
            pboard = psaved_board;
            if (e < best_e) {
                best_e = e;
                move_to_make = std::make_tuple(std::get<1>(vec), std::get<2>(vec));
            }
        }
    }
    */
    return move_to_make;

}
int main()
{
    initializeZobristKeys();
    std::vector<int> board{
        8,9,10,5,12,10,9,8,
        7,7,7,7,7,7,7,7,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,
        2,3,4,5,6,4,3,2, 0,0, 0,0,0, 0,0,0 };
    //64 and 65 are for last move made(for en passant) and ((66, 67, 68)(white), (69, 70,71)(black)) are for castling
        // Step 1: Register the window class
    const wchar_t* CLASS_NAME = L"MyWindowClass";

    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Get screen width and height
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Chessboard dimensions
    int boardSize = 8;
    int squareSize = 50;

    // Calculate the starting position to center the chessboard
    int boardWidth = boardSize * squareSize;
    int boardHeight = boardSize * squareSize;
    int x = (screenWidth - boardWidth) / 2;
    int y = (screenHeight - boardHeight) / 2;

    // Create the window with fullscreen style
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Chessboard Window", WS_POPUP | WS_VISIBLE,
        x, y, boardWidth, boardHeight,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (hwnd == nullptr) {
        return -1;
    }

    // Show and run the window
    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}