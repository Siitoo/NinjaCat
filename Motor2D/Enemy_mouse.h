#ifndef __ENEMY_MOUSE_H__
#define __ENEMY_MOUSE_H__

#include "Enemy.h"
#include "p2DynArray.h"
#include "p2Point.h"
enum Direction;

class Enemy_Mouse : public Enemy
{
private:
	
	Animation walkLeftFly;
	Animation walkRightFly;
	Animation deadFly;
	
public:
	iPoint originalpos;

	Enemy_Mouse(int x, int y);
	void Move();
	int i = 0;
	bool current_in_path = false;
	p2DynArray<iPoint> enemy_path;
	bool movement[4] = { true,true,true,true };
	int now = 0;
	
};

#endif