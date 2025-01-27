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


#include "ResourcesManager.h"


std::string ResourcesManager::ResourcesPath = "resources";
std::vector<std::string> ResourcesManager::ModelsExtensions = { ".obj" };
std::vector<std::string> ResourcesManager::TexturesExtensions = { ".png" };

std::vector<std::string> ResourcesManager::AllPaths;

std::vector<std::string> ResourcesManager::ModelsPaths;
std::vector<std::string> ResourcesManager::ModelsNames;
std::vector<Model> ResourcesManager::Models;
std::vector<Model> ResourcesManager::InstancedModels;

std::vector<std::string> ResourcesManager::TexturesPaths;
std::vector<std::string> ResourcesManager::TexturesNames;
std::vector<Texture2D> ResourcesManager::Textures;

std::vector<std::string> ResourcesManager::JsonsNames;

using Json = nlohmann::json;


// Returns a vector of file name strings in some directory with some extension.
static std::vector<std::string> GetDirectoryFiles(const fs::path& directory = fs::current_path(),
	const std::vector<std::string> extensions = {})
{
	// List of file names.
	std::vector<std::string> files{};

	for (const auto& file : fs::recursive_directory_iterator(directory))
	{
		if (fs::is_regular_file(file))
		{
			if (extensions.empty() ||
				std::find(extensions.begin(), extensions.end(), file.path().extension().string()) != (extensions.end()))
			{
				files.push_back(file.path().filename().string());
			}
		}
	}
	return files;
}


bool ResourcesManager::LoadResources()
{
	auto& path = fs::path{ fs::current_path() / ResourcesPath };
	std::vector<std::string> models{ GetDirectoryFiles(path, ModelsExtensions) };
	std::vector<std::string> textures{ GetDirectoryFiles(path, TexturesExtensions) };
	Models.clear(); //Meshes.reserve(meshes.size());
	Textures.clear(); Textures.reserve(textures.size());
	AllPaths.clear(); AllPaths.reserve(models.size() + textures.size());
	ModelsNames.clear(); ModelsNames.reserve(models.size());
	TexturesNames.clear(); TexturesNames.reserve(textures.size());

	int i = 0;
	for (auto& n : models)
	{
		std::string path{ ResourcesPath + "/" + n };
		std::string name{ strtok(n.data(), ".") };
		AllPaths.push_back(path);
		ModelsPaths.push_back(path);
		ModelsNames.push_back(name);
		std::cout << path << "\n";
		Models.push_back(LoadModel(path.c_str()));
	}
	//ModelsPaths.insert(0, "        ");

	for (auto& n : textures)
	{
		std::string path = ResourcesPath + "/" + n;
		std::string name{ strtok(n.data(), ".") };
		AllPaths.push_back(path);
		TexturesPaths.push_back(path);
		TexturesNames.push_back(name);
		std::cout << path << "\n";
		Textures.push_back(LoadTexture(path.c_str()));
	}

	//std::cout << AllPaths << "\n";

	// Jsons
	std::vector<std::string> jsons{ GetDirectoryFiles(path, { ".json" }) };
	for (auto& n : jsons)
	{
		std::string path = ResourcesPath + "/" + n;
		std::string name{ strtok(n.data(), ".") };
		Json j = Json::parse(std::ifstream(path));
		if (!j.contains("Prototypes")) continue;
		JsonsNames.push_back(name);
	}

	return true;
}


bool ResourcesManager::UnloadResources()
{
	for (auto& m : InstancedModels)
	{
		m.meshCount = 0;
		UnloadModel(m);
	}
	for (auto& m : Models)
		UnloadModel(m);
	for (auto& t : Textures)
		UnloadTexture(t);
	return true;
}


const std::vector<std::string>& ResourcesManager::GetModelsNames()
{
	return ModelsNames;
}

const std::vector<std::string>& ResourcesManager::GetTexturesNames()
{
	return TexturesNames;
}


Model ResourcesManager::GetInstancedModel(int PathIdx)
{
	auto& m = Models[PathIdx];
	auto im = LoadModelFromMesh(m.meshes[0]);
	// TODO: only uses the first mesh, not a problem for now
	//for (int i = 1; i < m.meshCount; i++)
	InstancedModels.push_back(im);
	return im;
}


std::string ResourcesManager::GetModelName(int PathIdx)
{
	return ModelsNames[PathIdx];
}


Texture2D ResourcesManager::GetTexture(int PathIdx)
{
	return Textures[PathIdx];
}
