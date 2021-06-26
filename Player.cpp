#include "Player.h"

//bool PlaceObject(GameObject *objects, int size, const std::string &path)
bool Level:: PlaceObject(const std::string &path)
{
  std::ifstream file(path);
  char c;
  int i=0;
  TileType type;

  if (file.is_open()) {
    std::cout << "*** Level file is opened [" << path << "]\n";
    while (file.get(c)) {
      //std::cout << c;
      if (c != '\n') {
        objects[i].coords_on_screen.x = x0 + (i % tileWidth) * tileSize;
        objects[i].coords_on_screen.y = height + y0 - (i / tileHeight) * tileSize - tileSize;
        objects[i].coords_on_level.x = (i % tileWidth) * tileSize;
        objects[i].coords_on_level.y = (i / tileHeight) * tileSize;
        switch(c) {
          case '.':
            type = FLOOR;
            objects[i].icon = floor_icon;
            break;
          case '#':
            type = WALL;
            objects[i].icon = wall_icon;
            break;
          case 'T':
            type = TRAP;
            objects[i].icon = trap_icon;
            if (std::rand() % 10 == 0)
              objects[i].icon = floor_icon;
            break;
          case 'x':
            type = EXIT;
            objects[i].icon = exit_icon;
            break;
          case ' ':
            type = VOID;
            objects[i].icon = void_icon;
            break;
          case '@':
            type = PLAYER;
            objects[i].icon = floor_icon;
            if (start_pos.x != -1 && start_pos.y != -1)
              std::cout << "\n*** WARN: LEVEL::PlaceObject start position was defined, first start_pos will be in use\n";
            else {
              start_pos.x = objects[i].coords_on_screen.x;
              start_pos.y = objects[i].coords_on_screen.y;
            }
            break;
        }
        //std::cout << i << '\n';
        objects[i].type = type;
        i++;
      }
    }
    if (i != size) {
      std::cout << "*** ERROR " << path << ":: count of objects [" << i << "] != size of level [" << size << "]\n";
    }
  } else {
    std::cout << "*** Level file is NOT opened! [" << path << "]\n";
    return false;
  }
  file.close();
  std::cout << "*** Level file is closed [" << path << "]\n";
  return true;
}

Level::Level(int x, int y, const std::string &path)
{
  x0 = x; y0 = y;
  x1 = x0 + tileWidth * tileSize;
  y1 = y0 + tileHeight * tileSize;
  height = y1 - y0;
  width = x1 - x0;

  size = tileWidth * tileHeight;
  objects = new GameObject[size];

  std::cout << "*** LEVEL GENERATED x0=" << x0 << " y0=" << y0 << " x1=" << x1 << " y1=" << y1 << std::endl;
  std::cout << "*** size or count of tiles = " << size << std::endl;

  if (!PlaceObject(path))
    std::cout << "*** ERROR "<< path << " unable to open or unresolved objects\n";

  image = new Image(x1 - x0, y1 - y0, 4);
  Image empty("resources/empty.png");
  image->Fill(empty);

  for (int i=0; i<size; i++) {
    //if ((i) % (tileWidth) == 0) std::cout << "\n";
    //std::cout << objects[i].type; // << "[" << i << "]";
    if (objects[i].icon == NULL) {
      std::cout << "*** WARN objects[" << i << "].icon unable to draw, it is NULL\n";
    } else {
      objects[i].icon->DrawReverse((i % tileWidth) * tileSize, (i / tileHeight) * tileSize, image);
      // objects[i].x = x0 + (i % tileWidth) * tileSize;
      // objects[i].y = height + y0 - (i / tileHeight) * tileSize - tileSize;
      // std::cout << "[" << i % tileWidth << "," << (i / tileWidth) << "]";
      // std::cout << "[" << objects[i].coords_on_screen.x << "," << objects[i].coords_on_screen.y << "]"; // coord-s on the screenBuffer
      // image->PutPixel(objects[i].coords_on_level.x - x0, objects[i].coords_on_level.y - y0, Pixel{.r = 255, .g = 0, .b = 0, .a = 255});
    }
  }
  //std::cout << "\n";
}

bool Level::NewLevel(const std::string &path)
{
  if (!objects || !image) {
    std::cout << "*** ERROR: Level::NewLevel: undefined previos level objects or image\n";
    return false;
  } else {
    start_pos.x=-1; 
    start_pos.y=-1;

    if (!PlaceObject(path))
      std::cout << "*** ERROR "<< path << " unable to open or unresolved objects\n";

    Image empty("resources/empty.png");
    image->Fill(empty);
    for (int i=0; i<size; i++) {
      if (objects[i].icon == NULL) {
        std::cout << "*** WARN objects[" << i << "].icon unable to draw, it is NULL\n";
      } else {
        objects[i].icon->DrawReverse((i % tileWidth) * tileSize, (i / tileHeight) * tileSize, image);
      }
    }
  }
  return true;
}

void Level::Draw(Image &screen)
{
  image->Draw(x0, y0, screen);
}

Pixel Level::GetPixel(int x, int y)
{
  return image->GetPixel(x - x0, height - (y - y0 + 1));
}

TileType Level::GetType(int x, int y)
{
  // TODO it is triggered when player checks move availability
  TileType t = UNDEFINED;
  int i = tileWidth * (tileHeight - (y - y0) / tileSize - 1) + (x - x0) / tileSize; // index on the tile grid
  t = objects[i].type;

  if (t == TRAP) { // trap animation
    objects[i].icon = trap_icon_active;
    objects[i].icon->DrawReverse((i % tileWidth) * tileSize, (i / tileHeight) * tileSize, image);
  }

  return t;
}

Player::Player(Level *lvl, GameStage game_stage) : icon(Image("resources/player.png"))
{
  // height = icon.Height(); width = icon.Width();
  height = 15; width = 15;

  stage = game_stage;
  if (!lvl)
    std::cout << "***WARN: PLAYER::LEVEL UNDEFINED\n";
  else {
    level = lvl;
    coords = level->GetStart();
    old_coords = coords;
    if (coords.x == -1 || coords.y == -1)
      std::cout << "***WARN: PLAYER::START POS UNDEFINED\n";
  }
  switch_level = false;

  
}

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

void Player::ShowStage()
{
  std::cout << "*** Player::stage == ";
  switch(stage)
  {
    case DEATH:
      std::cout << "DEATH\n";
      break;
    case LEVEL1:
      std::cout << "LEVEL1\n";
      break;
    case LEVEL2:
      std::cout << "LEVEL2\n";
      break;
    case END:
      std::cout << "END\n";
      break;
  }
}

void Player::ShowState()
{
  std::cout << "*** Player::state == ";
  switch(state)
  {
    case IDLE_LEFT:
      std::cout << "IDLE_LEFT\n";
      break;
    case IDLE_RIGHT:
      std::cout << "IDLE_RIGHT\n";
      break;
    case RUN_LEFT:
      std::cout << "RUN_LEFT\n";
      break;
    case RUN_RIGHT:
      std::cout << "RUN_RIGHT\n";
      break;
    case DEAD:
      std::cout << "DEAD\n";
      break;
  }
}

void Player::ProcessInput(MovementDir dir)
{
  int move_dist = move_speed * 1;

  if (stage == END || stage == DEATH) {
    move_dist = 0;
    if (stage == DEATH)
      state = DEAD;
  } else {

  

    switch(dir)
    {
      case MovementDir::UP:
        old_coords.y = coords.y;
        if (MoveAvailable(coords.x, coords.y+height+move_dist)
            && MoveAvailable(coords.x+width/2, coords.y+height+move_dist)
            && MoveAvailable(coords.x+width, coords.y+height+move_dist)) {
          coords.y += move_dist;
          state = RUN_LEFT;
        } else {
          state = IDLE_LEFT;
        }
        break;
      case MovementDir::DOWN:
        old_coords.y = coords.y;
        if (MoveAvailable(coords.x, coords.y-move_dist)
            && MoveAvailable(coords.x+width/2, coords.y-move_dist)
            && MoveAvailable(coords.x+width, coords.y-move_dist)) {
          coords.y -= move_dist;
          state = RUN_RIGHT;
        } else {
          state = IDLE_RIGHT;
        }
        break;
      case MovementDir::LEFT:
        old_coords.x = coords.x;
        if (MoveAvailable(coords.x-move_dist, coords.y)
              && MoveAvailable(coords.x-move_dist, coords.y+height/2)
              && MoveAvailable(coords.x-move_dist, coords.y+height)) {
          coords.x -= move_dist;
          state = RUN_LEFT;
        } else {
          state = IDLE_LEFT;
        }
        break;
      case MovementDir::RIGHT:
        old_coords.x = coords.x;
        if (MoveAvailable(coords.x+width+move_dist, coords.y)
              && MoveAvailable(coords.x+width+move_dist, coords.y+height/2)
              && MoveAvailable(coords.x+width+move_dist, coords.y+height)) {
          coords.x += move_dist;
          state = RUN_RIGHT;
        } else {
          state = IDLE_RIGHT;
        }
        break;
      default:
        break;
    }

    TileType t = level->GetType(coords.x + width/2, coords.y + height/2);
    if (switch_level == false 
        && t == EXIT) {
      switch_level = true;
      std::cout << "*** PLAYER:: I STEPPED ON EXIT\n";
    }

    if ((t == TRAP || t == VOID)
        && stage != DEATH) {
      // TODO make another icon, maybe I must use pointer
      //    - made trap animation in GetType
      //    - I wanna use sprites
      stage = DEATH;
      state = DEAD;
      ShowStage();
    }
  }
    
}

bool Player::MoveAvailable(int x, int y)
{
  // border points
  // (x0, y0);
  // (x0, y0 + height/2)
  // (x0, y0 + height);
  // (x0 + width/2, y0 + height);
  // (x0 + width, y0 + height);
  // (x0 + width, y0 + height/2);
  // (x0 + width, y0);
  // (x0 + width/2, y0);
  if (level->GetType(x, y) == WALL) { // in coord-s of screenBuffer
    return false;
  }
  return true;
}

void Player::Draw(Image &screen)
{
  if(Moved())
  {
    for(int y = old_coords.y; y <= old_coords.y + icon.Height(); ++y)
    {
      for(int x = old_coords.x; x <= old_coords.x + icon.Width(); ++x)
      {
        screen.PutPixel(x, y, level->GetPixel(x, y));
      }
    }
    old_coords = coords;
  }

  int y0 = coords.y, x0 = coords.x;
  for(int y = coords.y; y < y0 + icon.Height(); ++y)
  {
    for(int x = coords.x; x < x0 + icon.Width(); ++x)
    {
      screen.PutPixel(x, y, icon.GetPixel(x - x0, icon.Height() - (y - y0 + 1)));
      /*
      screen.PutPixel(x0, y0, color);
      screen.PutPixel(x0, y0 + height/2, color);
      screen.PutPixel(x0, y0 + height, color);
      screen.PutPixel(x0 + width/2, y0 + height, color);
      screen.PutPixel(x0 + width, y0 + height, color);
      screen.PutPixel(x0 + width, y0 + height/2, color);
      screen.PutPixel(x0 + width, y0, color);
      screen.PutPixel(x0 + width/2, y0, color);
      */
    }
  }
}

bool Player::NewLevel(GameStage game_stage)
{
  stage = game_stage;
  ShowStage();
  if (stage != END) {
    coords = level->GetStart();
    old_coords = coords;
  }
  switch_level = false;

  state = IDLE_RIGHT;
  return true;
}