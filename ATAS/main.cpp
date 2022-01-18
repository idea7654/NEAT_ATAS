// ATAS.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <vector>
#include "Game.h"
#include "RandWell.h"

using namespace std;

#define COLUMNS 60
#define ROWS 60

#define FPS 60

void timer_callback(int);
void display_callback();
void reshape_callback(int, int);
void keyboard_callback(int, int, int);

vector<Wall*> walls;
vector<Enemy*> enemies;
User *user;

int main(int argc, char **argv)
{
	Init_WELL_RAND();
	Wall *wall_1 = new Wall(0, 0, 60, 5); //bottom
	Wall *wall_2 = new Wall(0, 55, 60, 5); // top
	Wall *wall_3 = new Wall(0, 0, 5, 60); // left
	Wall *wall_4 = new Wall(55, 0, 5, 60); // right
	walls.push_back(wall_1);
	walls.push_back(wall_2);
	walls.push_back(wall_3);
	walls.push_back(wall_4);

	user = new User(10, 10);

	for (int i = 0; i < 10; i++)
	{
		int x = randbtn(5, 55);
		int y = randbtn(30, 55);
		Enemy *enemy = new Enemy(x, y);
		enemies.push_back(enemy);
	}
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(350, 40);

	glutCreateWindow("ATAS");
	glutDisplayFunc(display_callback);
	glutReshapeFunc(reshape_callback);
	glutTimerFunc(0, timer_callback, 0);
	glutSpecialFunc(keyboard_callback);

	glutMainLoop();

	for (auto &i : walls)
		delete i;
	delete user;

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

	user->drawUser();
	user->UpdateGunPos();
	user->gun->DrawGun();

	for (auto &i : enemies)
	{
		i->drawEnemy();
		i->UpdateGunPos();
		i->gun->DrawGun();
	}

	if (user->gun->bullets.size() > 0)
	{
		for (auto &i : user->gun->bullets)
		{
			i->MoveBullet();
			i->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
			i->drawBullet();
		}
		user->gun->DestoryBullet();
	}

	for (auto &i : enemies)
	{
		if (i->gun->bullets.size() > 0)
		{
			for (auto &j : i->gun->bullets)
			{
				j->MoveBullet();
				j->CheckRemoveBullet(walls[2], walls[3], walls[0], walls[1]);
				j->drawBullet();
			}
			i->gun->DestoryBullet();
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
		user->gun->Shoot();
	}

	if (key == GLUT_KEY_RIGHT)
	{
		user->RotateUser(1);
	}

	if (key == GLUT_KEY_LEFT)
	{
		user->MoveUser(2.0f, 2.5f);
	}
}