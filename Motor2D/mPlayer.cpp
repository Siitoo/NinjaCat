#include "p2Defs.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "mCollision.h"
#include "mPlayer.h"
#include "j1Audio.h"
#include "j1Module.h"
#include "p2Log.h"
#include "Animation.h"
#include "j1Map.h"

ModulePlayer::ModulePlayer()
{
	idleRight.PushBack({ 0,227,57,109 });
	idleRight.PushBack({ 59,227,57,109 });
	idleRight.PushBack({ 117,227,57,109 });
	idleRight.PushBack({ 175,227,57,109 });
	idleRight.speed = 0.1f;
	
	idleLeft.PushBack({ 237,227,57,109 });
	idleLeft.PushBack({ 295,227,57,109 });
	idleLeft.PushBack({ 353,227,57,109 });
	idleLeft.PushBack({ 411,227,57,109 });
	idleLeft.speed = idleRight.speed;

	walkRight.PushBack({ 9,3,81,107});
	walkRight.PushBack({ 99,2,80,110 });
	walkRight.PushBack({ 189,0,79,111 });
	//walkRight.PushBack({ 273,1,87,105 });
	//walkRight.PushBack({ 365,3,88,106 });
	walkRight.PushBack({ 463,3,81,105 });
	walkRight.PushBack({ 553,1,79,110 });
	walkRight.PushBack({ 643,0,78,107 });
	walkRight.PushBack({ 734,1,78,100 });
	walkRight.PushBack({ 826,3,81,104 });
	walkRight.speed = 0.2f;

	walkLeft.PushBack({ 1,117,80,107 });
	walkLeft.PushBack({ 93,115,80,112 });
	walkLeft.PushBack({ 186,115,79,110 });
	walkLeft.PushBack({ 275,115,87,104 });
	walkLeft.PushBack({ 364,117,87,106 });
	walkLeft.PushBack({ 455,117,80,105 });
	walkLeft.PushBack({ 547,115,80,109 });
	walkLeft.PushBack({ 640,114,78,107 });
	walkLeft.PushBack({ 730,115,78,100 });
	walkLeft.PushBack({ 818,117,80,104 });
	walkLeft.speed = walkRight.speed;

	climb.PushBack({ 9,344,52,94});
	climb.PushBack({ 68,344,52,94 });
	climb.PushBack({ 127,344,52,94 });
	climb.PushBack({ 184,344,52,94 });
	climb.PushBack({ 241,344,52,94 });
	climb.PushBack({ 303,344,52,94 });
	climb.PushBack({ 363,344,52,94 });
	climb.PushBack({ 419,344,52,94 });
	climb.speed = 0.2f;

	glide.PushBack({ 472,225,102,107 });
	glide.PushBack({ 579,224,102,107 });
	glide.PushBack({ 686,224,102,107 });
	glide.PushBack({ 793,227,102,107 });
	glide.PushBack({ 472,341,102,107 });
	glide.PushBack({ 579,341,102,107 });
	glide.PushBack({ 687,344,102,107 });
	glide.PushBack({ 795,345,102,107 });
	glide.speed = 0.2f;

	jump.PushBack({ 0,466,70,109 });
	jump.PushBack({ 88,464,66,114 });
	jump.PushBack({ 176,463,62,108 });
	jump.PushBack({ 265,463,63,107 });
	jump.PushBack({ 355,463,63,107 });
	jump.PushBack({ 445,463,63,107 });
	jump.PushBack({ 535,463,68,106 });
	jump.PushBack({ 620,463,74,104 });
	jump.PushBack({ 706,463,82,102 });
	jump.PushBack({ 796,463,82,102 });
	jump.speed = 0.1f;
	jump.loop = true;

	currentAnimation = &idleRight;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	graphics = App->tex->Load("maps/spriteSheet.png");

	position.x = 100;
	position.y = 300;

	colliderPlayer = App->collision->AddCollider({position.x,position.y,230,300},COLLIDER_PLAYER,this);

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	
	App->tex->UnLoad(graphics);
	
	colliderPlayer->to_delete = true;

	return true;
}

// Update: draw background
bool ModulePlayer::Update(float dt)
{
	int speed = 1;

	App->map->CollisionToWorld(colliderPlayer->rect);
	if (App->input->GetKey(SDL_SCANCODE_D)==KEY_REPEAT && movement[0] == true)
	{
		position.x += speed;
		currentAnimation = &walkRight;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && movement[1] == true)
	{
		position.x -= speed;
		currentAnimation = &walkLeft;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && movement[2]==true)
	{
		position.y -= speed;
		currentAnimation = &climb;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && movement[3] == true)
	{
		position.y += speed;
		currentAnimation = &glide;
		
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && movement[3] == true)
	{
		currentAnimation = &jump;

	}
	colliderPlayer->SetPos(position.x, position.y);
	App->render->Blit(graphics, position.x, position.y, &(currentAnimation->GetCurrentFrame()));
	
	return true;
}


void ModulePlayer::OnCollision(Collider* c1, Collider* c2)
{
	
}