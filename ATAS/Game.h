#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glut.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <mutex>
#include <chrono>

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

extern mutex m;

class Bullet
{
public:
	float x;
	float y;
	float forwardVec[2];
	bool isDestroy = false;
	bool isUnder;
	Character *WhoShoot;
	Character *WhoDamaged;
	float origin_x;
	float origin_y;

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

			float distance = (origin_x - x) * (origin_x - x) + (origin_y - y) * (origin_y - y);
			if (distance > 900)
				this->isDestroy = true;
		}
		//m.unlock();
	}

	void drawBullet()
	{
		if (isUnder)
			glColor3f(0, 0, 1);
		else
			glColor3f(1, 1, 0);
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

extern vector<Bullet*> Gbullets2;
extern vector<Bullet*> Gbullets_ThreadSafe2;

extern vector<Bullet*> Gbullets3;
extern vector<Bullet*> Gbullets_ThreadSafe3;

extern vector<Bullet*> Gbullets4;
extern vector<Bullet*> Gbullets_ThreadSafe4;

extern vector<Bullet*> Gbullets5;
extern vector<Bullet*> Gbullets_ThreadSafe5;

extern vector<Bullet*> Gbullets6;
extern vector<Bullet*> Gbullets_ThreadSafe6;

extern vector<Bullet*> Gbullets7;
extern vector<Bullet*> Gbullets_ThreadSafe7;

extern vector<Bullet*> Gbullets8;
extern vector<Bullet*> Gbullets_ThreadSafe8;

class Gun
{
public:
	float x;
	float y;
	float angle;
	float width = 0.5;
	float c_angle = 0.0;
	int height = 1;
	double RateOfShoot = 1.0;
	chrono::system_clock::time_point last_shoot;
	bool isUnder = false;
	double inputRotate;
	Character *Whohas;
	vector<Bullet*> bullets;
	vector<Bullet*> bullet_for_threadsafe;
	int roomNum = 0;

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
			glRotatef(-(angle - 90 + c_angle), 0, 0, 1);
		else
			//glRotatef(inputRotate, 0, 0, 1);
			glRotatef(angle - 90 + c_angle, 0, 0, 1);
		glTranslatef(-(x + 0.125), -(y - 0.5), 0);
		glRectd(x, y, x + width, y + height);
		glEnd();
		glPopMatrix();
	}

	void Shoot(int roomNum)
	{
		m.lock();
		chrono::duration<double> sec = chrono::system_clock::now() - last_shoot;
		if (sec.count() > RateOfShoot)
		{
			Bullet *newBullet = new Bullet();
			newBullet->x = x + 0.125;
			newBullet->y = y;
			newBullet->origin_x = x + 0.125;
			newBullet->origin_y = y;
			newBullet->WhoShoot = Whohas;
			newBullet->isUnder = isUnder;
			if (!isUnder)
			{
				newBullet->forwardVec[0] = cos((c_angle - 270) * 3.14159 / 180 + (angle - 90) * 3.14159 / 180) / 8;
				newBullet->forwardVec[1] = sin((c_angle - 270) * 3.14159 / 180 + (angle - 90) * 3.14159 / 180) / 8;
			}
			else
			{
				newBullet->forwardVec[0] = -cos((c_angle - 90) * 3.14159 / 180 + (angle - 270) * 3.14159 / 180) / 8;
				newBullet->forwardVec[1] = sin((c_angle - 90) * 3.14159 / 180 + (angle - 270) * 3.14159 / 180) / 8;
			}

			bullet_for_threadsafe.emplace_back(newBullet);
			switch (roomNum) //roomNum이 7만 들어오는 사태..
			{
			case 0:
			{
				Gbullets.emplace_back(newBullet);
				break;
			}
			case 1:
			{
				Gbullets2.emplace_back(newBullet);
				break;
			}
			case 2:
			{
				Gbullets3.emplace_back(newBullet);
				break;
			}
			case 3:
			{
				Gbullets4.emplace_back(newBullet);
				break;
			}
			case 4:
			{
				Gbullets5.emplace_back(newBullet);
				break;
			}
			case 5:
			{
				Gbullets6.emplace_back(newBullet);
				break;
			}
			case 6:
			{
				Gbullets7.emplace_back(newBullet);
				break;
			}
			case 7:
			{
				Gbullets8.emplace_back(newBullet);
				break;
			}
			default:
				cout << "Error" << endl;
				break;
			}
			
			last_shoot = chrono::system_clock::now();
		}
		m.unlock();
	}

	void DestroyBullet()
	{
		//m.lock();
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

		if (bullet_for_threadsafe.size() > 0)
		{
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
		}
		
		if (bullets.size() > 0)
		{
			for (vector<Bullet*>::size_type i = 0; i < bullets.size();)
			{
				if (bullets[i]->isDestroy && bullets[i] != nullptr)
				{
					bullets.erase(bullets.begin() + i);
				}
				else
				{
					i++;
				}
			}
		}

		switch (roomNum)
		{
		case 0:
		{
			if (Gbullets_ThreadSafe.size() > 0)
			{
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
			}

			if (Gbullets.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets.size();)
				{
					if (Gbullets[i] != nullptr)
					{
						if (Gbullets[i]->isDestroy)
						{
							Gbullets.erase(Gbullets.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		case 1:
		{
			if (Gbullets_ThreadSafe2.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe2.size();)
				{
					if (Gbullets_ThreadSafe2[i]->isDestroy)
					{
						Gbullets_ThreadSafe2.erase(Gbullets_ThreadSafe2.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}

			if (Gbullets2.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets2.size();)
				{
					if (Gbullets2[i] != nullptr)
					{
						if (Gbullets2[i]->isDestroy)
						{
							Gbullets2.erase(Gbullets2.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		case 2:
		{
			if (Gbullets_ThreadSafe3.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe3.size();)
				{
					if (Gbullets_ThreadSafe3[i]->isDestroy)
					{
						Gbullets_ThreadSafe3.erase(Gbullets_ThreadSafe3.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}

			if (Gbullets3.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets3.size();)
				{
					if (Gbullets3[i] != nullptr)
					{
						if (Gbullets3[i]->isDestroy)
						{
							Gbullets3.erase(Gbullets3.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		case 3:
		{
			if (Gbullets_ThreadSafe4.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe4.size();)
				{
					if (Gbullets_ThreadSafe4[i]->isDestroy)
					{
						Gbullets_ThreadSafe4.erase(Gbullets_ThreadSafe4.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}

			if (Gbullets4.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets4.size();)
				{
					if (Gbullets4[i] != nullptr)
					{
						if (Gbullets4[i]->isDestroy)
						{
							Gbullets4.erase(Gbullets4.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		case 4:
		{
			if (Gbullets_ThreadSafe5.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe5.size();)
				{
					if (Gbullets_ThreadSafe5[i]->isDestroy)
					{
						Gbullets_ThreadSafe5.erase(Gbullets_ThreadSafe5.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}

			if (Gbullets5.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets5.size();)
				{
					if (Gbullets5[i] != nullptr)
					{
						if (Gbullets5[i]->isDestroy)
						{
							Gbullets5.erase(Gbullets5.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		case 5:
		{
			if (Gbullets_ThreadSafe6.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe6.size();)
				{
					if (Gbullets_ThreadSafe6[i]->isDestroy)
					{
						Gbullets_ThreadSafe6.erase(Gbullets_ThreadSafe6.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}

			if (Gbullets6.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets6.size();)
				{
					if (Gbullets6[i] != nullptr)
					{
						if (Gbullets6[i]->isDestroy)
						{
							Gbullets6.erase(Gbullets6.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		case 6:
		{
			if (Gbullets_ThreadSafe7.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe7.size();)
				{
					if (Gbullets_ThreadSafe7[i]->isDestroy)
					{
						Gbullets_ThreadSafe7.erase(Gbullets_ThreadSafe7.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}

			if (Gbullets7.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets7.size();)
				{
					if (Gbullets7[i] != nullptr)
					{
						if (Gbullets7[i]->isDestroy)
						{
							Gbullets7.erase(Gbullets7.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		case 7:
		{
			if (Gbullets_ThreadSafe8.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets_ThreadSafe8.size();)
				{
					if (Gbullets_ThreadSafe8[i]->isDestroy)
					{
						Gbullets_ThreadSafe8.erase(Gbullets_ThreadSafe8.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}

			if (Gbullets8.size() > 0)
			{
				for (vector<Bullet*>::size_type i = 0; i < Gbullets8.size();)
				{
					if (Gbullets8[i] != nullptr)
					{
						if (Gbullets8[i]->isDestroy)
						{
							Gbullets8.erase(Gbullets8.begin() + i);
						}
						else
						{
							i++;
						}
					}
				}
			}
			break;
		}
		default:
			break;
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
		//m.unlock();
	}
}; // 지우기만 하면됨

class Character
{
public:
	float x;
	float y;
	Gun *gun;
	double angle = 0.0;
	float left_angle = 0.0f;
	float right_angle = 0.0f;
	double c_angle = 0.0;
	bool isDie = false;
	bool isUnder = false;
	int hp = 100;
	int sameDirCount = 0;
	bool previousDir; // false - Left, true - Right

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
		gun->last_shoot = chrono::system_clock::now();
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
		//if (this->y > 40)
		//	this->y = 40;
		//if (this->y < 4)
		//	this->y = 4;
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
		glRotatef(-c_angle, 0, 0, 1);
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
		//float sum = 0;
		c_angle -= sum;
		if (c_angle > 360)
			c_angle -= 360;
		if (c_angle < 0)
			c_angle += 360;

		this->x += cosf((c_angle - 90) / 180 * 3.14159) / 12;
		this->y -= sinf((c_angle - 90) / 180 * 3.14159) / 12;
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
		//if (this->y <= 60)
		//	this->y = 60;
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
		glRotatef(-c_angle, 0, 0, 1);
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
		//float sum = 0.1;
		c_angle += sum;
		if (c_angle > 360)
			c_angle -= 360;
		if (c_angle < 0)
			c_angle += 360;

		this->x += cosf((c_angle - 270) * 3.14159 / 180) / 12;
		this->y -= sinf((c_angle - 270) * 3.14159 / 180) / 12;
	}

	void RotateCannon(double value)
	{
		angle += value;
	}
};