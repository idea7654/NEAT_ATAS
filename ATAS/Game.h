#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glut.h>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

#define RADIUS 1;

class Character;

class Wall
{
public:
	int x;
	int y;
	int width;
	int height;

	void drawWall()
	{
		glColor3f(1.0, 0.0, 0.0);
		glRectd(x, y, x + width, y + height);
	}

	Wall(int px, int py, int pwidth, int pheight)
	{
		x = px;
		y = py;
		width = pwidth;
		height = pheight;
	}

	/*void Collision(Character *character)
	{
		if (isInteract(character->x - 2, character->y, 4, 2, this->x, this->y, this->width, this->height))
			cout << "Collision!" << endl;
	}

	bool isInteract(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
	{
		if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2)
			return true;
		return false;
	}
	*/
};

enum Gun_Type
{
	NORMAL = 0,
};

class Bullet
{
public:
	float x;
	float y;
	float forwardVec[2];
	bool isDestroy = false;

	void CheckRemoveBullet(Wall *left, Wall *right, Wall *bottom, Wall *top)
	{
		if (x < left->x + 5 || x > right->x || y > top->y || y < bottom->y + 5)
		{
			isDestroy = true;
		}
	}

	void MoveBullet()
	{
		x += forwardVec[0];
		y += forwardVec[1];
	}

	void drawBullet()
	{
		glColor3f(0, 0, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i <= 300; i++) {
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) / 2 * RADIUS;
			double py = sin(angle) / 2 * RADIUS;
			glVertex2d(px + x, py + y);
		}
		glEnd();
	}
};

class Gun
{
public:
	float x;
	float y;
	float angle;
	float width = 0.5;
	int height = 1;
	float RateOfShoot;
	vector<Bullet*> bullets;

	~Gun()
	{
		for (auto &i : bullets)
		{
			delete i;
		}
	}

	void DrawGun()
	{
		glColor3f(1.0, 1.0, 1.0);
		
		glPushMatrix();
		glTranslatef(x + 0.125, y - 0.5, 0);
		glRotatef(angle - 90, 0, 0, 1);
		glTranslatef(-(x + 0.125), -(y - 0.5), 0);
		glRectd(x, y, x + width, y + height);
		glEnd();
		glPopMatrix();
	}

	void Shoot()
	{
		Bullet *newBullet = new Bullet();
		newBullet->x = x + 0.125;
		newBullet->y = y;

		newBullet->forwardVec[0] = cos(angle  * 3.14159 / 180);
		newBullet->forwardVec[1] = sin(angle  * 3.14159 / 180);
		bullets.push_back(newBullet);
	}

	void DestoryBullet()
	{
		vector<Bullet *> removeList;
		for (auto &i : bullets)
		{
			if (i->isDestroy)
				removeList.push_back(i);
		}

		bullets.erase(remove_if(begin(bullets), end(bullets), [&](Bullet *bullet) {
			return bullet->isDestroy == true;
			}), bullets.end());

		if (removeList.size() > 0)
		{
			for (auto &i : removeList)
			{
				delete i;
			}
		}
		//메모리 삭제도 이루어져야함.
	}

	void GunRotate()
	{

	}
};

class Character
{
public:
	float x;
	float y;
	Gun *gun;
	float angle = 0.0f;
	float left_angle = 0.0f;
	float right_angle = 0.0f;
	float c_angle = 0.0f;
};

class User : public Character
{
public:
	User(int px, int py)
	{
		x = px;
		y = py;
		gun = new Gun();
		angle = 90;
	}

	~User()
	{
		delete gun;
	}

	void UpdateGunPos()
	{
		gun->x = x - 0.125;
		gun->y = y + 0.5;
		gun->angle = angle;
	}

	void drawUser()
	{
		glColor3f(0, 0, 1);
		//glBegin(GL_POLYGON);
		/*for (int i = 0; i <= 300; i++) {
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) * RADIUS;
			double py = sin(angle) * RADIUS;
			glVertex2d(px + x, py + y);
		}*/
		glRectd(x - 1.0f, y - 1.0f, x + 1.0f, y + 1.0f);
		glEnd();
		//body

		glColor3f(0, 0, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i <= 300; i++) 
		{
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) / 1.5 * RADIUS;
			double py = sin(angle) / 1.5 * RADIUS;
			glVertex2d(px + x - 1.75f, py + y);
		}
		glEnd(); //left tire

		glColor3f(0, 0, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i <= 300; i++) 
		{
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) / 1.5 * RADIUS;
			double py = sin(angle) / 1.5 * RADIUS;
			glVertex2d(px + x + 1.75f, py + y);
		}
		glEnd(); //right tire

		
	}

	void MoveUser(float left, float right)
	{
		float sum = right - left;
		c_angle += sum;
		if (c_angle < 0)
		{
			this->x -= cosf(c_angle) * 2;
			this->y -= sinf(c_angle) * 2;
		}
		else
		{
			this->x += cosf(c_angle) * 2;
			this->y += sinf(c_angle) * 2;
		}
	}

	void RotateUser(int value)
	{
		angle += value;
	}
};

class Enemy : public Character
{
public:
	Enemy(int px, int py)
	{
		x = px;
		y = py;
		gun = new Gun();
		angle = 270;
	}

	Enemy()
	{
		gun = new Gun();
	}

	~Enemy()
	{
		delete gun;
	}

	void UpdateGunPos()
	{
		gun->x = x - 0.125;
		gun->y = y + 0.5;
		gun->angle = angle;
	}

	void drawEnemy()
	{
		glColor3f(1, 0, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i <= 300; i++) {
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) * RADIUS;
			double py = sin(angle) * RADIUS;
			glVertex2d(px + x, py + y);
		}
		glPushMatrix();
		glRotatef(1, x, y, 1);
		glPopMatrix();
		glFlush();
		glEnd();
	}

	void RotateEnemy(int value)
	{
		angle += value;
	}
};