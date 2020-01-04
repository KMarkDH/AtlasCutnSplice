#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/_types.h>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <map>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

typedef unsigned char* byte;
struct Image
{
    int width,height, channel;
    byte data;
};

bool getParams(const int &argc,char* argv[], char* dirpath, int &margin)
{
    if (argc < 3)
    {
        std::cout << "usage: pngsplice <dirpath> <margin>" << std::endl;
        return false;
    }

    const char* rpath = realpath(argv[1], NULL);
    strcpy(dirpath, rpath);
    margin = abs(atoi(argv[2]));
    return true;
}

void fillEmptyPixel(byte data, const int &tHeight, const int &margin)
{
    for (int i = 0; i < tHeight; ++i)
    {
        memset(&data[i * tHeight * 4], 0, margin * 4);
    }
}

void openDir(const char* dirpath, std::vector<std::string> &fileList)
{
    DIR *dir;
    struct dirent *ent;
    std::map<int, std::string> fileSet;
    if ((dir = opendir(dirpath)) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") == 0)
                continue;

            if (strcmp(ent->d_name, "..") == 0)
                continue;

            if (ent->d_name[0] == '.')
                continue;

            if (ent->d_type == DT_REG)
            {
                std::stringstream ss;
                ss << dirpath << '/' << ent->d_name;
                fileSet.insert(std::make_pair(atoi(ent->d_name), ss.str()));
            }
        }
    }

    for (auto iter = fileSet.begin(); iter != fileSet.end(); ++iter)
    {
        fileList.push_back(iter->second);
    }
}

void readPng(std::string filepath, std::vector<Image*> &imagelist, const int &margin, int &twidth, int &theight)
{
    std::vector<std::string> filelist;
    openDir(filepath.c_str(), filelist);

    for (auto iter = filelist.begin(); iter != filelist.end(); ++iter)
    {
        int width, height, channel;
        byte data = stbi_load((*iter).c_str(), &width, &height, &channel, 0);
        Image *img = new Image();
        img->width = width;
        img->height = height;
        img->channel = channel;
        img->data = data;

        twidth += width + margin * 2;
        theight = height > theight ? height : theight;
        imagelist.push_back(img);
    }
}

void freePng(std::vector<Image*> &imagelist)
{
    for (auto iter = imagelist.begin(); iter != imagelist.end(); ++iter)
    {
        stbi_image_free((*iter)->data);
        delete (*iter);
    }
    imagelist.clear();
}

unsigned char* splicePng(const std::vector<Image*> & imagelist, const int &twidth, const int &theight, const int &margin)
{
    unsigned char *tData = new unsigned char[twidth * theight * 4];
    memset(tData, 0, twidth * theight * 4);
    int offset = 0;
    for (auto iter = imagelist.cbegin(); iter != imagelist.cend(); ++iter)
    {
        Image *img = *iter;
        for (int i = 0; i < theight; ++i)
        {
            if (i < img->height)
            {
                byte trow = &tData[i * twidth * 4 + offset];
                byte srow = &img->data[i * img->width * 4];
                memset(trow, 0, margin * 4);
                memcpy(trow + margin * 4, srow, img->width * 4);
                memset(trow + img->width * 4 + margin * 4, 0, margin * 4);
            }
            else
            {
                byte trow = &tData[i * twidth * 4 + offset];
                memset(trow, 0, margin * 4);
                memset(trow + margin * 4, 0, img->width * 4);
                memset(trow + img->width * 4 + margin * 4, 0, margin * 4);
            }
        }
        offset += img->width * 4 + margin * 8;
    }
    return tData;
    
}

char* getOutPutFileName(char *dir)
{
    
    char *filename = strrchr(dir, '/');
    char upDir[512]; 
    sprintf(upDir, "%s/..", dir);
    const char* rPath = realpath(upDir, NULL);

    std::stringstream ss;
    ss << rPath << filename << "-spl.png";

    char *ret = new char[ss.str().size()];
    strcpy(ret, ss.str().c_str());
    return ret;
}

int main(int argc, char *argv[])
{
    
    char filepath[512];
    int margin;
    if (!getParams(argc, argv, filepath, margin))
    {
        return -1;
    }
    
    std::vector<Image*> tArray;
    int twidth = 0, theight = 0;
    readPng(filepath, tArray, margin, twidth, theight);

    unsigned char *tData = splicePng(tArray, twidth, theight, margin);
    char *wpath = getOutPutFileName(filepath);
    stbi_write_png(wpath, twidth, theight, 4, tData, twidth * 4);
    std::cout << "output to : " << wpath << std::endl;

    delete[] wpath;
    freePng(tArray);
    return 0;
}