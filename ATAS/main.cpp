// ATAS.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

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

void timer_callback(int);
void display_callback();
void reshape_callback(int, int);
void keyboard_callback(int, int, int);

vector<Wall*> walls;
//vector<Enemy*> enemies;
//vector<User*> users;
//User *user;

vector<User*> users;
vector<Enemy*> enemies;
vector<int> initial_pos;
vector<Bullet*> Gbullets;
vector<Bullet*> Gbullets_ThreadSafe;
atomic<bool> GameOver;
atomic<bool> startNextGame = false;
atomic<int> user_hp_sum = 0;
atomic<int> enemy_hp_sum = 0;

bool isFirst = true;
bool joinFinish = false;

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
		users.push_back(user);
		initial_pos.push_back(x);
		initial_pos.push_back(y);
	}

	for (int i = 0; i < 3; i++)
	{
		int x = randbtn(15, COLUMNS - 15);
		int y = randbtn(60, COLUMNS - 15);

		Enemy *enemy = new Enemy(x, y);
		enemy->gun->Whohas = enemy;
		enemies.push_back(enemy);
		initial_pos.push_back(x);
		initial_pos.push_back(y);
	} //Enemy Spawn

	GameOver = false;
	startNextGame = true;

	future<void> future = async(launch::async, []() {
		tank_game(100);
		});

	auto gameManager =  async(launch::async, [&]() {
		while (true)
		{
			if (GameOver || isFirst)
			{
				joinFinish = false;
				isFirst = false;
				GameOver = false;
				Sleep(5000);
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
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					i->isDie = false;
					user_hp_sum += i->hp;
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
					i->gun->bullets.clear();
					i->gun->bullet_for_threadsafe.clear();
					i->isDie = false;
					enemy_hp_sum += i->hp;
					i->hp = 100;
					i->angle = 270;
					i->c_angle = 0.0f;
				}
				GameOver = true;
				startNextGame = true;
				Sleep(500); //Wait for all tank thread join
				Gbullets_ThreadSafe.clear();
				Gbullets.clear();
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

	for (auto &i : walls)
		delete i;
	//delete user;
	for (auto &i : users)
		delete i;

	for (auto &i : enemies)
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

	for (auto &i : users)
	{
		if (!i->isDie)
		{
			i->drawUser();
			i->UpdateGunPos();
			i->gun->DrawGun();
			i->UserCollider();
			i->gun->bullet_for_threadsafe = i->gun->bullets;
			i->gun->Shoot();
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
				
				i->gun->DestoryBullet();
				m.unlock();
			}
		}
	}

	for (auto &i : enemies)
	{
		//i->drawEnemy();
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
			i->gun->Shoot();
			i->gun->bullet_for_threadsafe = i->gun->bullets;
			Gbullets_ThreadSafe = Gbullets;
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
					j->drawBullet();
				}
				i->gun->DestoryBullet();
				m.unlock();
			}
		}
	}

	for (auto &i : Gbullets_ThreadSafe)
	{
		if (i->WhoShoot->isUnder)
		{
			for (auto &j : enemies)
			{
				//적들과의 충돌처리!
				if (!j->isDie)
				{
					if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
					{
						j->hp -= 20;
						i->isDestroy = true;
						if (j->hp <= 0)
						{
							j->isDie = true;
						}
					}
				}
			}
		}
		else
		{
			for (auto &j : users)
			{
				if (!j->isDie)
				{
					if (i->x - 1 + 2 >= j->x - 2 && i->x - 1 <= j->x - 2 + 4 && 2 + i->y - 1 >= j->y - 2 && i->y - 1 <= j->y - 2 + 4)
					{
						j->hp -= 20;
						i->isDestroy = true;

						if (j->hp <= 0)
						{
							//cout << "Die!" << endl;
							j->isDie = true;
						}
					}
				}
			}
		}
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
		enemies[0]->gun->Shoot();
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