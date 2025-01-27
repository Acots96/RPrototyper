/**********************************************************************************************
*
*   MIT License
*
*   Copyright (c) 2025 Aleix Cots Molina (@Acots96) (acots96@gmail.com)
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/


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
