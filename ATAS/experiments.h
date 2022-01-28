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

extern vector<User*> users;
extern vector<Enemy*> enemies;
extern vector<int> initial_pos;
extern atomic<bool> GameOver;
extern atomic<bool> startNextGame;
extern bool joinFinish;
extern atomic<int> user_hp_sum;
extern atomic<int> enemy_hp_sum;
static mutex mutex_2;
static vector<int> fitness_rank;
static atomic<int> user_fitness_sum = 0;
static atomic<int> enemy_fitness_sum = 0;

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
bool tank_evaluate(Organism *org, bool &isWin, int num);
int measure_fitness_tank(Population *pop, int generation, char *filename);
int try_tank(Network *net, int max_steps, int thresh, int num);