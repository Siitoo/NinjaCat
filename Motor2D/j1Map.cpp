#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>
#include "mPlayer.h"
#include "mCollision.h"
#include "j1Input.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;
	p2List_item<MapLayer*>* item_layer = data.layermap.start;
	p2List_item<TileSet*>* item_set= data.tilesets.start;
	while (item_layer->next != nullptr){


			for (int x = 0; x < item_layer->data->width; x++)
			{
				for (int y = 0; y < item_layer->data->height; y++)
				{
					int tileID = 0;

					if (x > item_layer->data->width || y > item_layer->data->height)
					{
						tileID = 0;
					}
					else {
						tileID = item_layer->data->Get(x, y);
					}

					if (tileID > 0)
					{
						iPoint position = MapToWorld(x, y);
						SDL_Rect rect = item_set->data->GetTileRect(tileID);
						if (item_layer->data->name == "Background") {
							if (App->player->movement[left]) {
								App->render->Blit(item_set->data->texture, position.x, position.y, &rect, 0.8);
							}
							else if (App->player->movement[right]) {
								App->render->Blit(item_set->data->texture, position.x, position.y, &rect, 0.8);
							}
							else {
								App->render->Blit(item_set->data->texture, position.x, position.y, &rect);
							}
						}
						else{
							App->render->Blit(item_set->data->texture, position.x, position.y, &rect);
						}
					}

				}
			}

			if (item_layer->next != nullptr) {
				item_layer = item_layer->next;

				if (item_set->next != nullptr) {
					item_set = item_set->next;
				}
			}
		}

}


iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;
	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const 
{
	iPoint ret;
	
	ret.x = x / data.tile_width;
	ret.y = y / data.tile_height;
	//Not finish

	/*ret.x =  x / data.tile_width + y/data.tile_width;
	ret.y =  y / data.tile_height - x/data.tile_height;*/

	return ret;

}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// TODO 2: clean up all layer data
	// Remove all layers
	p2List_item<MapLayer*>* litem;
	litem = data.layermap.start;
	while (litem != NULL)
	{
		RELEASE(litem->data);
		litem = litem->next;
	}
	data.tilesets.clear();
	data.layermap.clear();
	
	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();
		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// TODO 4: Iterate all layers and load each of them
	// Load layer info ----------------------------------------------
	for (pugi::xml_node layerset = map_file.child("map").child("layer"); layerset; layerset = layerset.next_sibling("layer"))
	{
		MapLayer* l = new MapLayer();
		if (ret == true)
		{
			LoadLayer(layerset, l);
		}
		data.layermap.add(l);

	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
	
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		// TODO 4: Add info here about your loaded layers
		// Adapt this vcode with your own variables
		
		p2List_item<MapLayer*>* item_layer = data.layermap.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

// TODO 3: Create the definition for a function that loads a single layer
bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	layer->name = node.attribute("name").as_string();
	layer->height = node.attribute("height").as_int();
	layer->width = node.attribute("width").as_int();


	for (pugi::xml_node dataLayer = node.child("data").child("tile"); dataLayer; dataLayer = dataLayer.next_sibling("tile"))
	{
		layer->gid.add(dataLayer.attribute("gid").as_int());
		
		LOG("%i", dataLayer.attribute("gid").as_int());
	}
	return true;

}

void j1Map::CollisionToWorld(SDL_Rect& playerRect, bool* movement)
{
	//collider wall gid 20

	// 130 is wall , 132 dead, 133 reespawn
	uint wall = 141, dead = 143, playerStart = 144, changeLvl = 145;
	MapLayer* layerCollision;
	if (data.layermap.start->next->next != nullptr)
	{
		layerCollision = data.layermap.start->next->next->data;

		iPoint rightUp = WorldToMap(playerRect.x + playerRect.w, playerRect.y);
		iPoint rightDown = WorldToMap(playerRect.x + playerRect.w-34, playerRect.y + playerRect.h);
		iPoint leftUp = WorldToMap(playerRect.x, playerRect.y);
		iPoint leftDown = WorldToMap(playerRect.x+34, playerRect.y + playerRect.h);

		int colliderRightUp = layerCollision->Get(rightUp.x, rightUp.y);
		int colliderRightDown = layerCollision->Get(rightDown.x, rightDown.y);
		int colliderLeftUp = layerCollision->Get(leftUp.x, leftUp.y);
		int colliderLeftDown = layerCollision->Get(leftDown.x, leftDown.y);

		int rightUpPlayer = layerCollision->Get(rightUp.x, rightUp.y);
		int rightDownPlayer = layerCollision->Get(rightDown.x, rightDown.y);
		int leftDownPlayer = layerCollision->Get(leftDown.x, leftDown.y);
		int leftUpPlayer = layerCollision->Get(leftUp.x, leftUp.y);
		//test 
		for(int i = up; i <= death; i++)
		switch (i)
		{
		case right:
		
			leftDownPlayer = layerCollision->Get(leftDown.x, leftDown.y);
			if (colliderRightUp == wall || colliderRightDown == wall)
			{
				
				if (wall == rightUpPlayer ||( wall == rightDownPlayer && leftDownPlayer != wall))
				{
					App->player->movement[right] = false;
				}
				else
				{
					App->player->movement[right] = true;
				}
			}
			else if (App->player->movement[right] == false)
			{
				App->player->movement[right] = true;
			}

			break;

		case left:
			
			if (colliderLeftUp == wall || colliderLeftDown == wall)
			{
				
				if (wall == leftUpPlayer || (colliderLeftDown==leftDownPlayer && rightDownPlayer != wall) )
				{
				App->player->movement[left] = false;
				}
				else
				{
					App->player->movement[left] = true;
				}
			}
			else if (App->player->movement[left] == false)
			{
				App->player->movement[left] = true;
			}
			break;


		case down:
		
			if (colliderLeftDown == wall && colliderRightDown == wall)
			{	
				if (colliderLeftDown == leftDownPlayer || colliderRightDown == rightDownPlayer)
				{
					App->player->movement[down] = false;
					App->player->jumping = false;
				}
			}
			else if (App->player->movement[down] == false)
			{
				
				App->player->movement[down] = true;
			}
			break;

		case up:
			if (colliderLeftUp == wall && colliderRightUp == wall)
			{
				if (colliderLeftUp == leftUpPlayer || colliderLeftUp == rightUpPlayer)
				{
					App->player->movement[up] = false;
				}
			}
			else if (App->player->movement[up] == false)
			{
				App->player->movement[up] = true;
			}
			break;

		case death:

			if (colliderLeftUp == dead || colliderRightUp == dead || colliderLeftDown == dead && colliderRightDown == dead)
			{
				if (now == 0) {
					now = SDL_GetTicks();
					
				}
				if (now + 800 > SDL_GetTicks()) {
					App->player->currentAnimation = &App->player->dead;
					movement[down] = false;
					movement[left] = false;
					movement[right] = false;
				}
				else {
					App->player->currentAnimation = &App->player->idleRight;
					now = 0;
					if (App->player->isLevel1)
					{
						App->player->needRespawn1 = true;

					}
					else
					{
						App->player->needRespawn2 = true;
					}
					App->player->movement[down] = false;
				}
			}
		case nextLevel:

			if (colliderRightDown == changeLvl || colliderRightUp == changeLvl)
			{
				App->player->changeLevel = true;
				if (App->player->isLevel1 == false)
				{
					App->player->isLevel1 = true;
				}
				else
				{
					App->player->isLevel1 = false;
				}
			}


		}

		

	}
}
