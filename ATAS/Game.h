#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glut.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <mutex>

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

static mutex m;

class Bullet
{
public:
	float x;
	float y;
	float forwardVec[2];
	bool isDestroy = false;
	Character *WhoShoot;
	Character *WhoDamaged;

	void CheckRemoveBullet(Wall *left, Wall *right, Wall *bottom, Wall *top)
	{
		if (x < left->x + 5 || x > right->x || y > top->y || y < bottom->y + 5)
		{
			isDestroy = true;
		}
	}

	void MoveBullet()
	{
		//m.lock();
		if (this != nullptr)
		{
			x += forwardVec[0];
			y += forwardVec[1];
		}
		//m.unlock();
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

extern vector<Bullet*> Gbullets;
extern vector<Bullet*> Gbullets_ThreadSafe;

class Gun
{
public:
	float x;
	float y;
	float angle;
	float width = 0.5;
	float c_angle = 0.0;
	int height = 1;
	float RateOfShoot;
	bool isUnder = false;
	double inputRotate;
	Character *Whohas;
	vector<Bullet*> bullets;
	vector<Bullet*> bullet_for_threadsafe;

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
		if (isUnder)
			//glRotatef(inputRotate, 0, 0, 1);
			glRotatef(-(angle - 90 + c_angle * 180 / 3.14159), 0, 0, 1);
		else
			//glRotatef(inputRotate, 0, 0, 1);
			glRotatef(angle - 90 + c_angle * 180 / 3.14159, 0, 0, 1);
		glTranslatef(-(x + 0.125), -(y - 0.5), 0);
		glRectd(x, y, x + width, y + height);
		glEnd();
		glPopMatrix();
	}

	void Shoot()
	{
		m.lock();
		Bullet *newBullet = new Bullet();
		newBullet->x = x + 0.125;
		newBullet->y = y;
		newBullet->WhoShoot = Whohas;
		if (!isUnder)
		{
			newBullet->forwardVec[0] = cos(c_angle - (270 * 3.14159 / 180) + (angle - 90) * 3.14159 / 180);
			newBullet->forwardVec[1] = sin(c_angle - (270 * 3.14159 / 180) + (angle - 90) * 3.14159 / 180);
		}
		else
		{
			newBullet->forwardVec[0] = -cos(c_angle - (90 * 3.14159 / 180) + (angle - 270) * 3.14159 / 180);
			newBullet->forwardVec[1] = sin(c_angle - (90 * 3.14159 / 180) + (angle - 270) * 3.14159 / 180);
		}
		
		bullets.emplace_back(newBullet);
		Gbullets.emplace_back(newBullet);

		m.unlock();
	}

	void DestoryBullet()
	{
		vector<Bullet *> removeList;
		for (auto &i : bullet_for_threadsafe)
		{
			if (i->isDestroy)
				removeList.push_back(i);
		}

		/*bullets.erase(remove_if(begin(bullets), end(bullets), [&](Bullet *bullet) {
			return bullet->isDestroy == true;
			}), bullets.end());

		Gbullets.erase(remove_if(begin(Gbullets), end(Gbullets), [&](Bullet *bullet) {
			if (bullet->isDestroy == true && bullet->WhoShoot == this->Whohas)
				return true;
			else
				return false;
			}), Gbullets.end());*/

		for (vector<Bullet*>::size_type i = 0; i < bullet_for_threadsafe.size();)
		{
			if (bullet_for_threadsafe[i]->isDestroy)
			{
				bullet_for_threadsafe.erase(bullet_for_threadsafe.begin() + i);
			}
			else
			{
				i++;
			}
		}

		for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe.size();)
		{
			if (Gbullets_ThreadSafe[i]->isDestroy)
			{
				Gbullets_ThreadSafe.erase(Gbullets_ThreadSafe.begin() + i);
			}
			else
			{
				i++;
			}
		}

		for (vector<Bullet*>::size_type i = 0; i < bullets.size();)
		{
			if (bullets[i]->isDestroy || bullets[i] == nullptr)
			{
				bullets.erase(bullets.begin() + i);
			}
			else
			{
				i++;
			}
		}

		for (vector<Bullet*>::size_type i = 0; i < Gbullets.size();)
		{
			if (Gbullets[i]->isDestroy || Gbullets[i] == nullptr)
			{
				Gbullets.erase(Gbullets.begin() + i);
			}
			else
			{
				i++;
			}
		}
		
		if (removeList.size() > 0)
		{
			for (auto &i : removeList)
			{
				if(i)
					delete(i);
			}
		}
		//메모리 삭제도 이루어져야함.
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
	bool isDie = false;
	bool isUnder = false;
	int hp = 100;

	void Collision(Wall *wall)
	{
		//if (this->x - 2.5 + 5 >= wall->x && this->x - 2.5 <= wall->x + wall->width && this->y - 2.5 + 5 >= wall->y && this->y - 2.5 <= wall->y + wall->height)
		//{
		//	//cout << "충돌!!" << endl;
		//	this->isDie = true;
		//}
		if (x < 7)
		{
			x = 93;
		}
		if (x > 93)
		{
			x = 7;
		}
		if (y > 93)
		{
			y = 7;
		}
		if (y < 7)
		{
			y = 93;
		}
	}
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
		gun->c_angle = c_angle;
	}

	void UserCollider()
	{
		if (this->y > 40)
			this->y = 40;
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
		
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(c_angle * 180 / 3.14159, 0, 0, 1);
		glTranslatef(-x , -y, 0);
		glRectd(x - 1.0f, y - 1.0f, x + 1.0f, y + 1.0f);
		
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
		//glEnd(); //left tire

		glColor3f(0, 0, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i <= 300; i++) 
		{
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) / 1.5 * RADIUS;
			double py = sin(angle) / 1.5 * RADIUS;
			glVertex2d(px + x + 1.75f, py + y);
		}
		//glEnd(); //right tire

		glEnd();
		glPopMatrix();
	}

	void MoveUser(float left, float right)
	{
		float sum = right - left;
		c_angle += sum;
		if (c_angle < 0)
		{
			this->x -= cosf(c_angle - 90);
			this->y -= sinf(c_angle - 90);
		}
		else
		{
			this->x += cosf(c_angle + 90);
			this->y += sinf(c_angle + 90);
		}
	}

	void RotateCannon(double value)
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
		gun->x = x;
		gun->y = y + 0.5;
		gun->angle = angle;
		gun->c_angle = c_angle;
		gun->isUnder = true;
	}

	void EnemyCollider()
	{
		if (this->y <= 60)
			this->y = 60;
	}

	void drawUser()
	{
		glColor3f(1, 0, 1);
		//glBegin(GL_POLYGON);
		/*for (int i = 0; i <= 300; i++) {
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) * RADIUS;
			double py = sin(angle) * RADIUS;
			glVertex2d(px + x, py + y);
		}*/

		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(-c_angle * 180 / 3.14159, 0, 0, 1);
		glTranslatef(-x, -y, 0);
		glRectd(x - 1.0f, y - 1.0f, x + 1.0f, y + 1.0f);
		glEnd();

		//body

		glColor3f(0, 1, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i <= 300; i++)
		{
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) / 1.5 * RADIUS;
			double py = sin(angle) / 1.5 * RADIUS;
			glVertex2d(px + x - 1.75f, py + y);
		}
		glEnd(); //left tire

		glColor3f(0, 1, 1);
		glBegin(GL_POLYGON);
		for (int i = 0; i <= 300; i++)
		{
			double angle = 2 * 3.14159 * i / 300;
			double px = cos(angle) / 1.5 * RADIUS;
			double py = sin(angle) / 1.5 * RADIUS;
			glVertex2d(px + x + 1.75f, py + y);
		}
		//glEnd(); //right tire

		glEnd();
		glPopMatrix();
	}

	void MoveUser(float left, float right)
	{
		float sum = right - left;
		c_angle += sum;
		if (c_angle < 0)
		{
			this->x -= cosf(c_angle - 90);
			this->y += sinf(c_angle - 90);
		}
		else
		{
			this->x += cosf(c_angle + 90);
			this->y -= sinf(c_angle + 90);
		}
	}

	void RotateCannon(int value)
	{
		angle += value;
	}
};