#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>


Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, 4)) != nullptr)
  {
    size = width * height * channels;
    //std::cout << "***LOAD IMG" << std::endl;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ]{};

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}


int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

Pixel Image::GetPixel(int x, int y)
{
  //if (data[width * y + x].a)
    return data[width * y + x];
  //else
    //return backgroundColor;
}

void Image::PutPixel(int x, int y, const Pixel &pix)
{
  uint8_t alpha = pix.a;
  if (alpha) {
    data[width * y + x].r = (alpha*pix.r + (255-alpha)*data[width * y + x].r)/255;
    data[width * y + x].g = (alpha*pix.g + (255-alpha)*data[width * y + x].g)/255;
    data[width * y + x].b = (alpha*pix.b + (255-alpha)*data[width * y + x].b)/255;
    data[width * y + x].a = (alpha*pix.a + (255-alpha)*data[width * y + x].a)/255;
  } else {
    //data[width * y + x] = pix;
  }
}

void Image::Draw(int x0, int y0, Image &screen)
{
  for(int y = y0; y < y0 + height; ++y)
  {
    for(int x = x0; x < x0 + width; ++x)
    {
      screen.PutPixel(x, y, GetPixel(x - x0, height - (y - y0 + 1)));
    }
  }
}

void Image::Draw(int x0, int y0, Image *screen)
{
  for(int y = y0; y < y0 + height; ++y)
  {
    for(int x = x0; x < x0 + width; ++x)
    {
      screen->PutPixel(x, y, this->GetPixel(x - x0, height - (y - y0 + 1)));
    }
  }
}

void Image::DrawReverse(int x0, int y0, Image *screen)
{
  for(int y = y0; y < y0 + height; ++y)
  {
    for(int x = x0; x < x0 + width; ++x)
    {
      screen->PutPixel(x, y, this->GetPixel(x - x0, y - y0));
    }
  }
}

void Image::Fill(Image &img)
{ 
  for(int y = 0; y < height; ++y)
  {
    for(int x = 0; x < width; ++x)
    {
      PutPixel(x, y, img.GetPixel(x % img.Width(), img.Height() - (y % img.Height() + 1)));
    }
  }
}

void Image::FillReverse(Image &img)
{ 
  for(int y = 0; y < height; ++y)
  {
    for(int x = 0; x < width; ++x)
    {
      PutPixel(x, y, img.GetPixel(x % img.Width(), y % img.Height()));
    }
  }
}

void Image::Fill(const Pixel &pix)
{
  for(int y = 0; y < height; ++y)
  {
    for(int x = 0; x < width; ++x)
    {
      PutPixel(x, y, pix);
    }
  }
}

void Image::FillArea(int x0, int y0, int x1, int y1, Image &img)
{ 
  for(int y = y0; y < y1; ++y)
  {
    for(int x = x0; x < x1; ++x)
    {
      this->PutPixel(x, y, img.GetPixel((x - x0) % img.Width(), img.Height() - ((y - y0) % img.Height() + 1)));
    }
  }
}

void Image::FadeIn(GLFWwindow *window, Image &screen, float sec)
{
  GLfloat t0 = glfwGetTime();
  GLfloat dt = 0;
  float k = 0.0;
  Pixel pix {0, 0, 0, 255};

  while (dt < sec) {
    k = (1 / sec * dt);
    for(int y = 0; y < height; ++y)
    {
      for(int x = 0; x < width; ++x)
      {
        pix.r = data[width * y + x].r * k;
        pix.g = data[width * y + x].g * k;
        pix.b = data[width * y + x].b * k;
        pix.a = data[width * y + x].a * k;
        screen.PutPixel(x, y, pix);
      }
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels (screen.Width(), screen.Height(), GL_RGBA, GL_UNSIGNED_BYTE, screen.Data());
    glfwSwapBuffers(window);
    dt = glfwGetTime() - t0;
  }
}

void Image::FadeOut(GLFWwindow *window, Image &screen, float sec)
{
  GLfloat t0 = glfwGetTime();
  GLfloat dt = 0;

  float k = 1.0;

  Pixel pix {0, 0, 0, 255};

  while (dt < sec) {
    k = 1 - (1 / sec * dt);
    for(int y = 0; y < height; ++y)
    {
      for(int x = 0; x < width; ++x)
      {
        pix.r = data[width * y + x].r * k;
        pix.g = data[width * y + x].g * k;
        pix.b = data[width * y + x].b * k;
        screen.PutPixel(x, y, pix);
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels (screen.Width(), screen.Height(), GL_RGBA, GL_UNSIGNED_BYTE, screen.Data());
    glfwSwapBuffers(window);
    dt = glfwGetTime() - t0;
  }

  
}

Image::~Image()
{
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}