#ifndef __J1FADETOBLACK_H__
#define __J1FADETOBLACK_H__

#include "j1Module.h"
#include "j1Scene.h"
#include "SDL\include\SDL_rect.h"

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	virtual ~j1FadeToBlack();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	bool FadeToBlack(j1Module*, j1Module*, actual_level, float);
	bool IsFading() const;

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	actual_level level_fading_to;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
	j1Module* m_on = nullptr;
	j1Module* m_off = nullptr;
};

#endif //__MODULEFADETOBLACK_H__