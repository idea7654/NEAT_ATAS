// ATAS.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma warning(disable:4996)
#include <iostream>
#include <vector>
#include <future>
#include <mutex>
#include "Game.h"
#include "RandWell.h"
#include "neat.h"
#include "population.h"
#include "experiments.h"

using namespace std;

#define COLUMNS 100
#define ROWS 100

#define FPS 60
#define GAME_TIME 25000

void timer_callback(int);
void display_callback();
void reshape_callback(int, int);
void keyboard_callback(int, int, int);

vector<Wall*> walls;
//vector<Enemy*> enemies;
//vector<User*> users;
//User *user;

mutex m;

//-------- Room1 ------------//
vector<User*> users;
vector<Enemy*> enemies;
vector<int> initial_pos;
vector<Bullet*> Gbullets;
vector<Bullet*> Gbullets_ThreadSafe;
atomic<bool> GameOver;
atomic<bool> startNextGame = false;
atomic<int> user_hp_sum = 0;
atomic<int> enemy_hp_sum = 0;
atomic<int> user_fitness_sum = 0;
atomic<int> enemy_fitness_sum = 0;

bool isFirst = true;

//-------- Room2 ------------//
vector<User*> users2;
vector<Enemy*> enemies2;
vector<int> initial_pos2;
vector<Bullet*> Gbullets2;
vector<Bullet*> Gbullets_ThreadSafe2;
atomic<bool> GameOver2;
atomic<bool> startNextGame2 = false;
atomic<int> user_hp_sum2 = 0;
atomic<int> enemy_hp_sum2 = 0;
atomic<int> user_fitness_sum2 = 0;
atomic<int> enemy_fitness_sum2 = 0;

bool isFirst2 = true;

//-------- Room3 ------------//
vector<User*> users3;
vector<Enemy*> enemies3;
vector<int> initial_pos3;
vector<Bullet*> Gbullets3;
vector<Bullet*> Gbullets_ThreadSafe3;
atomic<bool> GameOver3;
atomic<bool> startNextGame3 = false;
atomic<int> user_hp_sum3 = 0;
atomic<int> enemy_hp_sum3 = 0;
atomic<int> user_fitness_sum3 = 0;
atomic<int> enemy_fitness_sum3 = 0;

bool isFirst3 = true;

//-------- Room4 ------------//
vector<User*> users4;
vector<Enemy*> enemies4;
vector<int> initial_pos4;
vector<Bullet*> Gbullets4;
vector<Bullet*> Gbullets_ThreadSafe4;
atomic<bool> GameOver4;
atomic<bool> startNextGame4 = false;
atomic<int> user_hp_sum4 = 0;
atomic<int> enemy_hp_sum4 = 0;
atomic<int> user_fitness_sum4 = 0;
atomic<int> enemy_fitness_sum4 = 0;

bool isFirst4 = true;

//-------- Room5 ------------//
vector<User*> users5;
vector<Enemy*> enemies5;
vector<int> initial_pos5;
vector<Bullet*> Gbullets5;
vector<Bullet*> Gbullets_ThreadSafe5;
atomic<bool> GameOver5;
atomic<bool> startNextGame5 = false;
atomic<int> user_hp_sum5 = 0;
atomic<int> enemy_hp_sum5 = 0;
atomic<int> user_fitness_sum5 = 0;
atomic<int> enemy_fitness_sum5 = 0;

bool isFirst5 = true;

//-------- Room6 ------------//
vector<User*> users6;
vector<Enemy*> enemies6;
vector<int> initial_pos6;
vector<Bullet*> Gbullets6;
vector<Bullet*> Gbullets_ThreadSafe6;
atomic<bool> GameOver6;
atomic<bool> startNextGame6 = false;
atomic<int> user_hp_sum6 = 0;
atomic<int> enemy_hp_sum6 = 0;
atomic<int> user_fitness_sum6 = 0;
atomic<int> enemy_fitness_sum6 = 0;

bool isFirst6 = true;

//-------- Room7 ------------//
vector<User*> users7;
vector<Enemy*> enemies7;
vector<int> initial_pos7;
vector<Bullet*> Gbullets7;
vector<Bullet*> Gbullets_ThreadSafe7;
atomic<bool> GameOver7;
atomic<bool> startNextGame7 = false;
atomic<int> user_hp_sum7 = 0;
atomic<int> enemy_hp_sum7 = 0;
atomic<int> user_fitness_sum7 = 0;
atomic<int> enemy_fitness_sum7 = 0;

bool isFirst7 = true;

//-------- Room8 ------------//
vector<User*> users8;
vector<Enemy*> enemies8;
vector<int> initial_pos8;
vector<Bullet*> Gbullets8;
vector<Bullet*> Gbullets_ThreadSafe8;
atomic<bool> GameOver8;
atomic<bool> startNextGame8 = false;
atomic<int> user_hp_sum8 = 0;
atomic<int> enemy_hp_sum8 = 0;
atomic<int> user_fitness_sum8 = 0;
atomic<int> enemy_fitness_sum8 = 0;
bool finishJoin8 = false;

bool isFirst8 = true;

int main(int argc, char **argv)
{
	Init_WELL_RAND();
	Wall *wall_1 = new Wall(0, 0, COLUMNS, 5); //bottom
	Wall *wall_2 = new Wall(0, COLUMNS - 5, COLUMNS, 5); // top
	Wall *wall_3 = new Wall(0, 0, 5, COLUMNS); // left
	Wall *wall_4 = new Wall(COLUMNS - 5, 0, 5, COLUMNS); // right
	walls.push_back(wall_1);
	walls.push_back(wall_2);
	walls.push_back(wall_3);
	walls.push_back(wall_4);

	NEAT::Population *p = 0;

	for (int i = 0; i < 3; i++)
	{
		int x = randbtn(15, COLUMNS - 15);
		int y = randbtn(15, COLUMNS / 2 - 10);

		User *user = new User(x, y);
		user->gun->Whohas = user;
		user->isUnder = true;
		user->roomNum = 0;
		users.push_back(user);
		initial_pos.push_back(x);
		initial_pos.push_back(y);

		User *user2 = new User(x, y);
		user2->gun->Whohas = user2;
		user2->isUnder = true;
		user2->roomNum = 1;
		users2.push_back(user2);
		initial_pos2.push_back(x);
		initial_pos2.push_back(y);

		User *user3 = new User(x, y);
		user3->gun->Whohas = user2;
		user3->isUnder = true;
		user3->roomNum = 2;
		users3.push_back(user3);
		initial_pos3.push_back(x);
		initial_pos3.push_back(y);

		User *user4 = new User(x, y);
		user4->gun->Whohas = user4;
		user4->isUnder = true;
		user4->roomNum = 3;
		users4.push_back(user4);
		initial_pos4.push_back(x);
		initial_pos4.push_back(y);

		User *user5 = new User(x, y);
		user5->gun->Whohas = user5;
		user5->isUnder = true;
		user5->roomNum = 4;
		users5.push_back(user5);
		initial_pos5.push_back(x);
		initial_pos5.push_back(y);

		User *user6 = new User(x, y);
		user6->gun->Whohas = user6;
		user6->isUnder = true;
		user6->roomNum = 5;
		users6.push_back(user6);
		initial_pos6.push_back(x);
		initial_pos6.push_back(y);

		User *user7 = new User(x, y);
		user7->gun->Whohas = user7;
		user7->isUnder = true;
		user7->roomNum = 6;
		users7.push_back(user7);
		initial_pos7.push_back(x);
		initial_pos7.push_back(y);

		User *user8 = new User(x, y);
		user8->gun->Whohas = user8;
		user8->isUnder = true;
		user8->roomNum = 7;
		users8.push_back(user8);
		initial_pos8.push_back(x);
		initial_pos8.push_back(y);
	}

	for (int i = 0; i < 3; i++)
	{
		int x = randbtn(15, COLUMNS - 15);
		int y = randbtn(60, COLUMNS - 15);

		Enemy *enemy = new Enemy(x, y);
		enemy->gun->Whohas = enemy;
		enemy->isUnder = false;
		enemy->roomNum = 0;
		enemies.push_back(enemy);
		initial_pos.push_back(x);
		initial_pos.push_back(y);

		Enemy *enemy2 = new Enemy(x, y);
		enemy2->gun->Whohas = enemy2;
		enemy2->isUnder = false;
		enemy2->roomNum = 1;
		enemies2.push_back(enemy2);
		initial_pos2.push_back(x);
		initial_pos2.push_back(y);

		Enemy *enemy3 = new Enemy(x, y);
		enemy3->gun->Whohas = enemy3;
		enemy3->isUnder = false;
		enemy3->roomNum = 2;
		enemies3.push_back(enemy3);
		initial_pos3.push_back(x);
		initial_pos3.push_back(y);

		Enemy *enemy4 = new Enemy(x, y);
		enemy4->gun->Whohas = enemy4;
		enemy4->isUnder = false;
		enemy4->roomNum = 3;
		enemies4.push_back(enemy4);
		initial_pos4.push_back(x);
		initial_pos4.push_back(y);

		Enemy *enemy5 = new Enemy(x, y);
		enemy5->gun->Whohas = enemy5;
		enemy5->isUnder = false;
		enemy5->roomNum = 4;
		enemies5.push_back(enemy5);
		initial_pos5.push_back(x);
		initial_pos5.push_back(y);

		Enemy *enemy6 = new Enemy(x, y);
		enemy6->gun->Whohas = enemy6;
		enemy6->isUnder = false;
		enemy6->roomNum = 5;
		enemies6.push_back(enemy6);
		initial_pos6.push_back(x);
		initial_pos6.push_back(y);

		Enemy *enemy7 = new Enemy(x, y);
		enemy7->gun->Whohas = enemy7;
		enemy7->isUnder = false;
		enemy7->roomNum = 6;
		enemies7.push_back(enemy7);
		initial_pos7.push_back(x);
		initial_pos7.push_back(y);

		Enemy *enemy8 = new Enemy(x, y);
		enemy8->gun->Whohas = enemy8;
		enemy8->isUnder = false;
		enemy8->roomNum = 7;
		enemies8.push_back(enemy8);
		initial_pos8.push_back(x);
		initial_pos8.push_back(y);
	} //Enemy Spawn

	GameOver = false;
	startNextGame = true;

	GameOver2 = false;
	startNextGame2 = true;

	GameOver3 = false;
	startNextGame3 = true;

	GameOver4 = false;
	startNextGame4 = true;

	GameOver5 = false;
	startNextGame5 = true;

	GameOver6 = false;
	startNextGame6 = true;

	GameOver7 = false;
	startNextGame7 = true;

	GameOver8 = false;
	startNextGame8 = true;

	future<void> future = async(launch::async, []() {
		tank_game(300);
		});

	auto gameManager = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver || isFirst)
			{
				isFirst = false;
				GameOver = false;
				Sleep(GAME_TIME);
				initial_pos.clear();
				//게임 재시작 로직
				for (auto &i : users)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos.push_back(x);
					initial_pos.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos.push_back(x);
					initial_pos.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver = true;
				startNextGame = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe.clear();
				Gbullets.clear();
				m.unlock();
			}
		}
		});
    
	auto gameManager2 = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver2 || isFirst2)
			{
				isFirst2 = false;
				GameOver2 = false;
				Sleep(GAME_TIME);
				initial_pos2.clear();
				//게임 재시작 로직
				for (auto &i : users2)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos2.push_back(x);
					initial_pos2.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum2 += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies2)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos2.push_back(x);
					initial_pos2.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum2 += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver2 = true;
				startNextGame2 = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe2.clear();
				Gbullets2.clear();
				m.unlock();
			}
		}
		});

	auto gameManager3 = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver3 || isFirst3)
			{
				isFirst3 = false;
				GameOver3 = false;
				Sleep(GAME_TIME);
				initial_pos3.clear();
				//게임 재시작 로직
				for (auto &i : users3)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos3.push_back(x);
					initial_pos3.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum3 += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies3)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos3.push_back(x);
					initial_pos3.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum3 += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver3 = true;
				startNextGame3 = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe3.clear();
				Gbullets3.clear();
				m.unlock();
			}
		}
		});

	auto gameManager4 = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver4 || isFirst4)
			{
				isFirst4 = false;
				GameOver4 = false;
				Sleep(GAME_TIME);
				initial_pos4.clear();
				//게임 재시작 로직
				for (auto &i : users4)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos4.push_back(x);
					initial_pos4.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum4 += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies4)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos4.push_back(x);
					initial_pos4.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum4 += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver4 = true;
				startNextGame4 = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe4.clear();
				Gbullets4.clear();
				m.unlock();
			}
		}
		});

	auto gameManager5 = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver5 || isFirst5)
			{
				isFirst5 = false;
				GameOver5 = false;
				Sleep(GAME_TIME);
				initial_pos5.clear();
				//게임 재시작 로직
				for (auto &i : users5)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos5.push_back(x);
					initial_pos5.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum5 += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies5)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos5.push_back(x);
					initial_pos5.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum5 += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver5 = true;
				startNextGame5 = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe5.clear();
				Gbullets5.clear();
				m.unlock();
			}
		}
		});

	auto gameManager6 = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver6 || isFirst6)
			{
				isFirst6 = false;
				GameOver6 = false;
				Sleep(GAME_TIME);
				initial_pos6.clear();
				//게임 재시작 로직
				for (auto &i : users6)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos6.push_back(x);
					initial_pos6.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum6 += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies6)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos6.push_back(x);
					initial_pos6.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum6 += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver6 = true;
				startNextGame6 = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe6.clear();
				Gbullets6.clear();
				m.unlock();
			}
		}
		});

	auto gameManager7 = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver7 || isFirst7)
			{
				isFirst7 = false;
				GameOver7 = false;
				Sleep(GAME_TIME);
				initial_pos7.clear();
				//게임 재시작 로직
				for (auto &i : users7)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos7.push_back(x);
					initial_pos7.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum7 += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies7)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos7.push_back(x);
					initial_pos7.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum7 += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver7 = true;
				startNextGame7 = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe7.clear();
				Gbullets7.clear();
				m.unlock();
			}
		}
		});

	auto gameManager8 = async(launch::async, [&]() {
		while (true)
		{
			if (GameOver8 || isFirst8)
			{
				isFirst8 = false;
				GameOver8 = false;
				Sleep(GAME_TIME);
				initial_pos8.clear();
				//게임 재시작 로직
				for (auto &i : users8)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(15, COLUMNS / 2 - 10);
					initial_pos8.push_back(x);
					initial_pos8.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//user_hp_sum8 += i->hp;
					i->hp = 100;
					i->angle = 90;
					i->c_angle = 0.0f;
				}
				for (auto &i : enemies8)
				{
					int x = randbtn(15, COLUMNS - 15);
					int y = randbtn(60, COLUMNS - 15);
					initial_pos8.push_back(x);
					initial_pos8.push_back(y);
					i->x = x;
					i->y = y;
					m.lock();
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					m.unlock();
					i->isDie = false;
					//enemy_hp_sum8 += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver8 = true;
				startNextGame8 = true;
				Sleep(500); //Wait for all tank thread join
				m.lock();
				Gbullets_ThreadSafe8.clear();
				Gbullets8.clear();
				m.unlock();
			}
		}
		});

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(350, 40);

	glutCreateWindow("ATAS");
	glutDisplayFunc(display_callback);
	glutReshapeFunc(reshape_callback);
	glutTimerFunc(0, timer_callback, 0);
	glutSpecialFunc(keyboard_callback);

	glutMainLoop();

	future.get();
	gameManager.get();
	gameManager2.get();
	gameManager3.get();
	gameManager4.get();
	gameManager5.get();
	gameManager6.get();
	gameManager7.get();
	gameManager8.get();

	for (auto &i : walls)
		delete i;
	//delete user;
	for (auto &i : users)
		delete i;

	for (auto &i : enemies)
		delete i;

	for (auto &i : users2)
		delete i;

	for (auto &i : enemies2)
		delete i;

	for (auto &i : users3)
		delete i;

	for (auto &i : enemies3)
		delete i;

	for (auto &i : users4)
		delete i;

	for (auto &i : enemies4)
		delete i;

	for (auto &i : users5)
		delete i;

	for (auto &i : enemies5)
		delete i;

	for (auto &i : users6)
		delete i;

	for (auto &i : enemies6)
		delete i;

	for (auto &i : users7)
		delete i;

	for (auto &i : enemies7)
		delete i;

	for (auto &i : users8)
		delete i;

	for (auto &i : enemies8)
		delete i;

	if (p)
		delete p;

	return 0;
}

void display_callback()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//Loop
	for (auto &i : walls)
	{
		i->drawWall();
		//i->Collision(user);
	}

	/*user->drawUser();
	user->UpdateGunPos();
	user->gun->DrawGun();*/
	Gbullets_ThreadSafe = Gbullets;
	Gbullets_ThreadSafe2 = Gbullets2;
	Gbullets_ThreadSafe3 = Gbullets3;
	Gbullets_ThreadSafe4 = Gbullets4;
	Gbullets_ThreadSafe5 = Gbullets5;
	Gbullets_ThreadSafe6 = Gbullets6;
	Gbullets_ThreadSafe7 = Gbullets7;
	Gbullets_ThreadSafe8 = Gbullets8;
	
	//요거 왜 안될까? 일단!!! shoot이 안됨 진짜루

	{ // ------ Room1 ----------
		for (auto &i : users)
		{
			i->gun->roomNum = 0;
			if (!i->isDie)
			{
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : enemies)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(0);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(0);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						//j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies)
		{
			i->gun->roomNum = 0;
			//i->drawEnemy();
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				bool shootTrue = false;
				for (auto &j : users)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(0);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(0);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						//j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							user_fitness_sum -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe)
		//{
		//	for (auto &j : enemies)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				//j -> enemy...isUnder = false
		//				if (i->WhoShoot->isUnder) //상대편이 쏜거에 맞았으면
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else //우리팀이 쏜거에 맞았으면...
		//				{
		//					i->isDestroy = true; //일단 없어지고..
		//					//체력은 냅두고 우리팀 fitness를 줄여야댐.
		//					enemy_fitness_sum -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	{ // ------ Room2 ----------
		for (auto &i : users2)
		{
			i->gun->roomNum = 1;
			if (!i->isDie)
			{
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : enemies2)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				///	i->gun->Shoot(1);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(1);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						//j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies2)
		{
			i->gun->roomNum = 1;
			//i->drawEnemy();
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : users2)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				///	i->gun->Shoot(1);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(1);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies2)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						//j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users2)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies2)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum2 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users2)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							user_fitness_sum2 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies2)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users2)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum2 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies2)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum2 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe2)
		//{
		//	for (auto &j : enemies2)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					enemy_fitness_sum2 -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users2)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum2 -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	{ // ------ Room3 ----------
		for (auto &i : users3)
		{
			i->gun->roomNum = 2;
			if (!i->isDie)
			{
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : enemies3)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(2);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(2);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						//j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies3)
		{
			i->gun->roomNum = 2;
			//i->drawEnemy();
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : users3)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				///	i->gun->Shoot(2);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(2);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies3)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						//j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users3)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies3)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum3 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users3)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							user_fitness_sum3 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies3)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users3)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum3 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies3)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum3 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe3)
		//{
		//	for (auto &j : enemies3)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					enemy_fitness_sum3 -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users3)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum3 -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	{ // ------ Room4 ----------
		for (auto &i : users4)
		{
			i->gun->roomNum = 3;
			if (!i->isDie)
			{
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : enemies4)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(3);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(3);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						//j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies4)
		{
			i->gun->roomNum = 3;
			//i->drawEnemy();
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : users4)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(3);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(3);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies4)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						//j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users4)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies4)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum4 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users4)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							user_fitness_sum4 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies4)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users4)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum4 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies4)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum4 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe4)
		//{
		//	for (auto &j : enemies4)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					enemy_fitness_sum4 -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users4)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum4 -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	{ // ------ Room5 ----------
		for (auto &i : users5)
		{
			i->gun->roomNum = 4;
			if (!i->isDie)
			{
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : enemies5)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(4);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(4);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						//j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies5)
		{
			i->gun->roomNum = 4;
			//i->drawEnemy();
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : users5)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(4);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(4);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies5)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						//j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users5)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies5)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum5 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users5)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							user_fitness_sum5 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies5)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users5)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum5 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies5)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum5 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe5)
		//{
		//	for (auto &j : enemies5)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					enemy_fitness_sum5 -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users5)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum5 -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	{ // ------ Room6 ----------
		for (auto &i : users6)
		{
			i->gun->roomNum = 5;
			if (!i->isDie)
			{
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : enemies6)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(5);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(5);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						//j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies6)
		{
			i->gun->roomNum = 5;
			//i->drawEnemy();
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : users6)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(5);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(5);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies6)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						//j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users6)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies6)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum6 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users6)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							user_fitness_sum6 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies6)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users6)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum6 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies6)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum6 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe6)
		//{
		//	for (auto &j : enemies6)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					enemy_fitness_sum6 -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users6)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum6 -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	{ // ------ Room7 ----------
		for (auto &i : users7)
		{
			i->gun->roomNum = 6;
			if (!i->isDie)
			{
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : enemies7)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(6);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(6);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						//j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies7)
		{
			i->gun->roomNum = 6;
			//i->drawEnemy();
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				//i->drawUser();
				i->UpdateGunPos();
				//i->gun->DrawGun();
				//i->gun->Shoot();
				bool shootTrue = false;
				for (auto &j : users7)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(6);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(6);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies7)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						//j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users7)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies7)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum7 -= 2;
							//cout << enemy_hp_sum7 << endl;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users7)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							user_fitness_sum7 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies7)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users7)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum7 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies7)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum7 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe7)
		//{
		//	for (auto &j : enemies7)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					enemy_fitness_sum7 -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users7)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum7 -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	{ // ------ Room8 ----------
		for (auto &i : users8)
		{
			i->gun->roomNum = 7;
			if (!i->isDie)
			{
				i->drawUser();
				i->UpdateGunPos();
				i->gun->DrawGun();
				i->UserCollider();
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				bool shootTrue = false;
				for (auto &j : enemies8)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}
				//if (shootTrue)
				//	i->gun->Shoot(7);
				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(7);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				for (auto &j : walls)
				{
					i->Collision(j);
				}

				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->MoveBullet();
						j->drawBullet();
					}
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &i : enemies8)
		{
			i->gun->roomNum = 7;
			if (!i->isDie)
			{
				for (auto &j : walls)
				{
					i->Collision(j);
				}
				i->EnemyCollider();
				i->drawUser();
				i->UpdateGunPos();
				i->gun->DrawGun();
				
				bool shootTrue = false;
				for (auto &j : users8)
				{
					double diffX = j->x - i->x;
					double diffY = j->y - i->y;
					double y = -sin(i->c_angle / 180 * 3.14159) * diffX + cos(i->c_angle / 180 * 3.14159) * diffY;
					double distance = diffX * diffX + diffY * diffY;
					if (y > 0 && distance < 1600)
						shootTrue = true;
				}

				chrono::duration<double> sec = chrono::system_clock::now() - i->gun->last_shoot;
				if (sec.count() > i->gun->RateOfShoot && shootTrue)
				{
					i->gun->Shoot(7);
					i->gun->last_shoot = chrono::system_clock::now();
				}
				//if (shootTrue)
				//{
				//	i->gun->Shoot(7);
				//}
				//i->gun->bullet_for_threadsafe = i->gun->bullets;
				//Gbullets_ThreadSafe = Gbullets;
			}
		}

		for (auto &i : enemies8)
		{
			if (!i->isDie)
			{
				if (i->gun->bullet_for_threadsafe.size() > 0)
				{
					m.lock();
					for (auto &j : i->gun->bullet_for_threadsafe)
					{
						j->MoveBullet();
						j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
						j->drawBullet();
					}
					//i->gun->DestoryBullet();
					m.unlock();
				}

				m.lock();
				i->gun->DestroyBullet();
				m.unlock();
			}
		}

		for (auto &k : users8)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : enemies8)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							enemy_hp_sum8 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : users8)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy =  true;
							user_fitness_sum8 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		for (auto &k : enemies8)
		{
			for (auto &i : k->gun->bullet_for_threadsafe)
			{
				for (auto &j : users8)
				{
					if (!j->isDie)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							//j->hp -= 2;
							user_hp_sum8 -= 2;
							i->isDestroy = true;
							if (j->hp <= 0)
							{
								j->isDie = true;
							}
						}
					}
				} //적이 맞은거
				for (auto &j : enemies8)
				{
					if (!j->isDie && i->WhoShoot != j)
					{
						if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
						{
							i->isDestroy = true;
							enemy_fitness_sum8 -= 10;
						}
					}
				} //자기팀 맞은거
			}
		}

		//for (auto &i : Gbullets_ThreadSafe8)
		//{
		//	for (auto &j : enemies8)
		//	{
		//		//적들과의 충돌처리!
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					enemy_fitness_sum8 -= 10;
		//				}
		//			}
		//		}
		//	}
		//	for (auto &j : users8)
		//	{
		//		if (!j->isDie && i->WhoShoot != j)
		//		{
		//			if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
		//			{
		//				if (!i->WhoShoot->isUnder)
		//				{
		//					j->hp -= 2;
		//					i->isDestroy = true;
		//					if (j->hp <= 0)
		//					{
		//						//cout << "Die!" << endl;
		//						j->isDie = true;
		//					}
		//				}
		//				else
		//				{
		//					i->isDestroy = true;
		//					user_fitness_sum8 -= 10;
		//				}
		//			}
		//		}
		//	}
		//}
	}

	glutPostRedisplay();
	glutSwapBuffers();
}

void reshape_callback(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, COLUMNS, 0.0, ROWS, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer_callback, 0);
}

void keyboard_callback(int key, int, int)
{
	if (key == GLUT_KEY_UP)
	{
		//user->gun->Shoot();
		//enemies[0]->gun->Shoot();
	}

	if (key == GLUT_KEY_RIGHT)
	{
		//user->RotateUser(1);
		//user->MoveUser(2.1f, 2.0f);
		///user->RotateUser(COLUMNS / 10000);
		enemies[0]->MoveUser(2.0f, 2.1f);
	}

	if (key == GLUT_KEY_LEFT)
	{
		//user->MoveUser(2.0f, 2.1f);
		//user->RotateUser(COLUMNS / 10000);
		enemies[0]->MoveUser(2.1f, 2.0f);
	}

	if (key == GLUT_KEY_DOWN)
	{
		//user->RotateCannon(1);
		enemies[0]->RotateCannon(1);
	}
}