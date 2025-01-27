#pragma once

#include "raylib.h"

#include <vector>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/stat.h>
namespace fs = std::filesystem;

#include <fstream>
#include "ThirdParty/nlohmann/json.hpp"


class ResourcesManager
{
	static std::string ResourcesPath;
	static std::vector<std::string> ModelsExtensions;
	static std::vector<std::string> TexturesExtensions;

	static std::vector<std::string> ModelsPaths;
	static std::vector<std::string> ModelsNames;
	static std::vector<Model> Models;
	static std::vector<Model> InstancedModels;

	static std::vector<std::string> TexturesPaths;
	static std::vector<std::string> TexturesNames;
	static std::vector<Texture2D> Textures;

	static std::vector<std::string> AllPaths;

	static std::vector<std::string> JsonsNames;

public:
	static bool LoadResources();
	static bool UnloadResources();

	static const std::vector<std::string>& GetModelsNames();
	static const std::vector<std::string>& GetTexturesNames();

	static Model GetInstancedModel(int PathIdx);
	static std::string GetModelName(int PathIdx);
	static Texture2D GetTexture(int PathIdx);
};
