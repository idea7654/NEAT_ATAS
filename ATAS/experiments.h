#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <atomic>
#include "neat.h"
#include "network.h"
#include "population.h"
#include "organism.h"
#include "genome.h"
#include "species.h"
#include "game.h"

using namespace std;

using namespace NEAT;

//static vector<Bird*> birds;
//static Bird *newBird;
//static Organism *maxOrganism;
//

//------------ Room1 ------------//
extern vector<User*> users;
extern vector<Enemy*> enemies;
extern vector<int> initial_pos;
extern atomic<bool> GameOver;
extern atomic<bool> startNextGame;
extern atomic<int> user_hp_sum;
extern atomic<int> enemy_hp_sum;
static mutex mutex_2;
static vector<int> fitness_rank;
static atomic<int> user_fitness_sum = 0;
static atomic<int> enemy_fitness_sum = 0;

//------------ Room2 ------------//
extern vector<User*> users2;
extern vector<Enemy*> enemies2;
extern vector<int> initial_pos2;
extern atomic<bool> GameOver2;
extern atomic<bool> startNextGame2;
extern atomic<int> user_hp_sum2;
extern atomic<int> enemy_hp_sum2;
static mutex mutex_3;
static vector<int> fitness_rank2;
static atomic<int> user_fitness_sum2 = 0;
static atomic<int> enemy_fitness_sum2 = 0;

//------------ Room3 ------------//
extern vector<User*> users3;
extern vector<Enemy*> enemies3;
extern vector<int> initial_pos3;
extern atomic<bool> GameOver3;
extern atomic<bool> startNextGame3;
extern atomic<int> user_hp_sum3;
extern atomic<int> enemy_hp_sum3;
static mutex mutex_4;
static vector<int> fitness_rank3;
static atomic<int> user_fitness_sum3 = 0;
static atomic<int> enemy_fitness_sum3 = 0;

//------------ Room4 ------------//
extern vector<User*> users4;
extern vector<Enemy*> enemies4;
extern vector<int> initial_pos4;
extern atomic<bool> GameOver4;
extern atomic<bool> startNextGame4;
extern atomic<int> user_hp_sum4;
extern atomic<int> enemy_hp_sum4;
static mutex mutex_5;
static vector<int> fitness_rank4;
static atomic<int> user_fitness_sum4 = 0;
static atomic<int> enemy_fitness_sum4 = 0;

//------------ Room5 ------------//
extern vector<User*> users5;
extern vector<Enemy*> enemies5;
extern vector<int> initial_pos5;
extern atomic<bool> GameOver5;
extern atomic<bool> startNextGame5;
extern atomic<int> user_hp_sum5;
extern atomic<int> enemy_hp_sum5;
static mutex mutex_6;
static vector<int> fitness_rank5;
static atomic<int> user_fitness_sum5 = 0;
static atomic<int> enemy_fitness_sum5 = 0;

//------------ Room6 ------------//
extern vector<User*> users6;
extern vector<Enemy*> enemies6;
extern vector<int> initial_pos6;
extern atomic<bool> GameOver6;
extern atomic<bool> startNextGame6;
extern atomic<int> user_hp_sum6;
extern atomic<int> enemy_hp_sum6;
static mutex mutex_7;
static vector<int> fitness_rank6;
static atomic<int> user_fitness_sum6 = 0;
static atomic<int> enemy_fitness_sum6 = 0;

//------------ Room7 ------------//
extern vector<User*> users7;
extern vector<Enemy*> enemies7;
extern vector<int> initial_pos7;
extern atomic<bool> GameOver7;
extern atomic<bool> startNextGame7;
extern atomic<int> user_hp_sum7;
extern atomic<int> enemy_hp_sum7;
static mutex mutex_8;
static vector<int> fitness_rank7;
static atomic<int> user_fitness_sum7 = 0;
static atomic<int> enemy_fitness_sum7 = 0;

//------------ Room8 ------------//
extern vector<User*> users8;
extern vector<Enemy*> enemies8;
extern vector<int> initial_pos8;
extern atomic<bool> GameOver8;
extern atomic<bool> startNextGame8;
extern atomic<int> user_hp_sum8;
extern atomic<int> enemy_hp_sum8;
static mutex mutex_9;
static vector<int> fitness_rank8;
static atomic<int> user_fitness_sum8 = 0;
static atomic<int> enemy_fitness_sum8 = 0;

Population *pole1_test(int gens);
bool pole1_evaluate(Organism *org);
int pole1_epoch(Population *pop, int generation, char *filename);
int go_cart(Network *net, int max_steps, int thresh); //Run input
//Move the cart and pole
void cart_pole(int action, float *x, float *x_dot, float *theta, float *theta_dot);
//
//Population *flappy_bird(int gens, Bird *bird);
//int measure_fitness_flappybird(Population *pop, int generation, char *filename);
//bool flappybird_evaluate(Organism *org);
//int try_flappybird(Network *net, int max_steps, int thresh); //Run input
//void GetMaxOrg(Organism* &org);

Population *tank_game(int gens);
bool tank_evaluate(Organism *org, bool &isWin, int num, int roomNum);
int measure_fitness_tank(Population *pop, int generation, char *filename);
int try_tank(Network *net, int max_steps, int thresh, int num, int roomNum);

atomic<bool> &findRoomGameOver(int roomNum);