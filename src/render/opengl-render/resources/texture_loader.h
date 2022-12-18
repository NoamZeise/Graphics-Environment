#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "../../render-shared/resources/resources.h"
#include <vector>

namespace Resource
{

class TextureLoader
{
public:
	TextureLoader();
	~TextureLoader();
	Texture LoadTexture(std::string path);
	Texture LoadTexture(unsigned char* data, int width, int height, int nrChannels);
	void Bind(Texture tex);
private:

struct LoadedTex
{
	LoadedTex(std::string path);
	LoadedTex(unsigned char* data, int width, int height, int nrChannels);
	~LoadedTex();
	void Bind();
	unsigned int ID;
	int width;
	int height;

private:
	void generateTexture(unsigned char* data, int width, int height, int nrChannels);
};

	std::vector<LoadedTex*> textures;
};


}


#endif
