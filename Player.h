#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include <iostream>
#include <fstream>

struct Point
{
  int x;
  int y;
};

enum GameStage
{
  DEATH,
  LEVEL1,
  LEVEL2,
  END
};

enum TileType
{
  UNDEFINED,
  FLOOR,
  WALL,
  TRAP,
  EXIT,
  VOID,
  PLAYER
};

struct GameObject
{
  // also you can add animation via coords
  Point coords_on_level {.x=-1, .y=-1};
  Point coords_on_screen {.x=-1, .y=-1};
  TileType type = UNDEFINED;
  Image *icon = NULL;
  // bool interactive = false;
};

class Level
{
  int tileHeight = 40; // count of tiles: on y
  int tileWidth = 40; // count of tiles: on x
  int tileSize = 16; // pixels

  // coord-s on the screenBuffer
  int x0, y0; // left bottom corner
  int x1, y1; // right upper corner
  int height, width;
  int size; // count of tiles

  Point start_pos {.x=-1, .y=-1};
  
  GameObject *objects = NULL;
  Image *image = NULL; // save after render

  //Image **floor_icon = new Image*[5];
  //Image floor_icon[0] = new Image("resources/floor1.png");
  Image *floor_icon = new Image("resources/floor.png");
  Image *wall_icon = new Image("resources/wall.png");
  Image *trap_icon = new Image("resources/trap.png");
  Image *trap_icon_active = new Image("resources/trap_active.png");
  Image *exit_icon = new Image("resources/exit.png");
  Image *void_icon = new Image("resources/void.png");

  bool PlaceObject(const std::string &path);
public:
  //need to save image after rendering with objects
  Level(int x, int y, const std::string &path);
  bool NewLevel(const std::string &path);

  void Draw(Image &screen);
  Pixel GetPixel(int x, int y);
  TileType GetType(int x, int y);
  Point GetStart() { return start_pos; }

  // ~Level() { if (objects) delete [] objects; }
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Player
{
  explicit Player(Level *lvl, GameStage game_stage);

  bool Moved() const;
  void ProcessInput(MovementDir dir);
  bool MoveAvailable(int x, int y);
  void Draw(Image &screen);
  void ShowStage();
  void ShowState();

  bool SwitchLevel() { return switch_level; }
  bool Dead() { return stage == DEATH; }
  bool NewLevel(GameStage game_stage);
  // void Switch() { switch_level = (switch_level + 1) % 2; }

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 0, .b = 0, .a = 255};
  int move_speed = 4;

  Image icon;
  int height;
  int width;
  GameStage stage; // stage of the game
  Level *level;
  bool switch_level = false;

  Image *sprite = NULL; // TODO ? maybe use another class with autodect of icons
  enum STATE {
    IDLE_LEFT,
    IDLE_RIGHT,
    RUN_LEFT,
    RUN_RIGHT,
    DEAD
  };
  STATE state = IDLE_RIGHT; // state for animation via a sprite
};

#endif //MAIN_PLAYER_H
