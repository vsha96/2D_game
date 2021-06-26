#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>

#ifndef GLFW_DLL
#define GLFW_DLL
#include <GLFW/glfw3.h>
#endif

constexpr int tileSize = 16;

struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

constexpr Pixel backgroundColor{0, 0, 0, 255};

struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return  data; }

  Pixel GetPixel(int x, int y);
  void  PutPixel(int x, int y, const Pixel &pix);
  void Draw(int x, int y, Image &screen);
  void Draw(int x, int y, Image *screen);
  void DrawReverse(int x, int y, Image *screen);
  void Fill(Image &img);
  void FillReverse(Image &img);
  void Fill(const Pixel &pix);
  void FillArea(int x0, int y0, int x1, int y1, Image &img);

  void FadeIn(GLFWwindow *window, Image &screen, float sec);
  void FadeOut(GLFWwindow *window, Image &screen, float sec);

  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  bool self_allocated = false;
};



#endif //MAIN_IMAGE_H
