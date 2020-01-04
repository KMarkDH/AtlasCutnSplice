#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/_types.h>
#include <sys/stat.h>
#include <sstream>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

typedef unsigned char* byte;

bool getParams(const int &argc,char* argv[], char* filepath, int &dwidth, int &dheight)
{
    if (argc < 4)
    {
        std::cout << "usage: pngcut <filepath> <width> <height>" << std::endl;
        return false;
    }

    const char* rpath = realpath(argv[1], NULL);
    strcpy(filepath, rpath);
    dwidth = abs(atoi(argv[2]));
    dheight = abs(atoi(argv[3]));
    return true;
}

unsigned char** allocTargetArray(const int& iw, const int& ih, const int& tw, const int& th, const int &com, int &arrSize)
{
    arrSize = (iw / tw) * (ih / th);
    byte* tArr = new unsigned char*[arrSize];
    for (int i = 0; i < arrSize; ++i)
    {
        tArr[i] = new unsigned char[tw * th * com];
        memset(tArr[i], 0, tw * th * com * sizeof(unsigned char));
    }
    return tArr;
}

void getPixel(byte data, byte dst, const int &width, const int &height, const int &x, const int &y)
{
    memcpy(dst, &data[(x + y * width) * 4], 4);
}

void pixelMapping(const int &sidx, const int &width, const int &tWidth, const int &tHeight, int &tidx, int &tx, int &ty, int &sx, int &sy)
{
        sy = sidx / width;
        sx = sidx - sy * width;

        int dx = sx / tWidth;
        int dy = sy / tHeight;

        tidx = dy * (width / tWidth) + dx;
    
        tx = sx - dx * tWidth;
        ty = sy - dy * tHeight;
}

void getWorkDir(char *filepath, char *workdir)
{
    char *filename = strrchr(filepath, '/');
    int plen = strlen(filepath);
    int flen = strlen(filename);
    char filedir[128];

    strncpy(workdir, filepath, plen - flen);

    char *ext = strrchr(filename, '.');
    int elen = strlen(ext);
    char folder[128];

    strncpy(folder, filename, flen - elen);
    strcat(workdir, folder);
    mkdir(workdir, 0775);
}

int main(int argc, char *argv[])
{
    
    char filepath[512];
    int dwidth, dheight;
    if (!getParams(argc, argv, filepath, dwidth, dheight))
    {
        return -1;
    }

    int width, height, channel;
    byte data = stbi_load(filepath, &width, &height, &channel, 0);
    if (NULL == data)
    {
        std::cout << "read png file error." << std::endl;
        return -1;
    }

    int arrSize = 0;
    byte* tarData = allocTargetArray(width, height, dwidth, dheight, channel, arrSize);

    byte pixelBuffer = new unsigned char[4];
    for (int i = 0; i < width * height; i++)
    {
        int tidx, tx, ty, sx, sy;
        pixelMapping(i, width, dwidth, dheight, tidx, tx, ty, sx, sy);
        byte tdata = tarData[tidx];

        getPixel(data, pixelBuffer, width, height, sx, sy);
        memcpy(&tdata[(tx + ty * dwidth) * 4], pixelBuffer, 4);
    }
  
    char workdir[512];
    getWorkDir(filepath, workdir);

    for (int i = 0; i < arrSize; ++i)
    {    
        std::stringstream ss;
        ss << workdir << "/";
        ss << i << ".png";
        stbi_write_png(ss.str().c_str(), dwidth, dheight, channel, tarData[i], dwidth * channel);
    }
    std::cout << "output : " << workdir << std::endl;

    stbi_image_free(data);
    return 0;
}