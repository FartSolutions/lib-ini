/*
  MIT License
  
  Copyright (c) 2023 Fürth.ORG
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#include "ini.h"

#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <regex>

#pragma warning(disable : 4018)

namespace
{
	bool exists(const std::string& path)
	{
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}

	template <typename T>
	std::basic_string<T> to_lower_case(const std::basic_string<T>& s)
	{
		std::basic_string<T> s2 = s;
		std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
		return s2;
	}

	bool starts_width(std::string string, std::string prefix)
	{
		if (string.rfind(prefix, prefix.size()) == 0)
		{
			return true;
		}
		return false;
	}

	bool ends_width(std::string const& fullString, std::string const& ending)
	{
		if (fullString.length() >= ending.length())
		{
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		}
		else
		{
			return false;
		}
	}

	std::vector<std::string> split(const std::string& input, const std::string& regex)
	{
		// passing -1 as the submatch index parameter performs splitting
		std::regex re(regex);
		std::sregex_token_iterator first{ input.begin(), input.end(), re, -1 }, last;
		return { first, last };
	}
} // anonymous

INIFile::INIFile()
{}

INIFile::INIFile(std::string filepath)
{
	LoadFile(filepath);
}

INIFile::~INIFile()
{
	if (!_categories.empty()) Clear();
}

int INIFile::LoadFile(std::string filepath)
{
	if (!exists(filepath))
	{
		return Failed;
	}
	
	_filepath = filepath;

	std::fstream file;
	file.open(filepath.c_str());
	if (file.is_open())
	{
		std::string line = "";
		while (std::getline(file, line))
		{
			if (!(line[0] == '#' || line[0] == ' ' || line[0] == '\t' || line.empty()))
			{
				if (line[0] == '[')
				{
					std::vector<std::string> category_str = split(split(line, "[")[1], "]");
					INICat categorie{};
					categorie.Name = to_lower_case(category_str[0]).c_str();
					_categories.push_back(categorie);
				}
				else
				{
					std::vector<std::string> splitted = split(line, "=");
					if (!_categories.empty())
					{
						INIKey key{};
						key.Name = to_lower_case(splitted[0]).c_str();
						key.Value = splitted[1].c_str();
						_categories[_categories.size() - 1].Keys.push_back(key);
					}
				}
			}
		}
	}

	return Success;
}

int INIFile::SaveFile(std::string filepath)
{
	std::string out_filepath = filepath.c_str();
	if (!filepath.empty()) out_filepath = _filepath;
	std::fstream file(out_filepath.c_str(), std::ios::out);

	if (file.is_open())
	{
		file << "# Created with lib_ini" << std::endl;
		for (auto cat : _categories)
		{
			if (!cat.Name.empty())
			{
				file << "[" << cat.Name.c_str() << "]" << std::endl;
				for (auto key : cat.Keys)
				{
					if (!key.Name.empty())
					{
						file << key.Name.c_str() << "=" << key.Value.c_str() << std::endl;
					}
				}
				file << std::endl;
			}
		}
		file.close();
	}

	return Success;
}

INIFile::INICat& INIFile::operator[](const std::string kat)
{
	std::string _kat = to_lower_case(std::string(kat.c_str())).c_str();
	
	int64_t kat_index{ -1 };
	for (int64_t i{ 0 }; i < _categories.size(); i++)
	{
		if (_categories[i].Name == kat)
		{
			kat_index = i;
		}
	}

	if (kat_index == -1)
	{
		INICat kategorie{};
		kategorie.Name = kat.c_str();
		_categories.push_back(kategorie);
		kat_index = _categories.size() - 1;
	}

	return _categories[kat_index];
}

std::string& INIFile::INICat::operator[](const std::string key)
{
	std::string key_name = to_lower_case(std::string(key.c_str())).c_str();

	int64_t key_index{ -1 };
	for (int64_t i{ 0 }; i < Keys.size(); i++)
	{
		if (Keys[i].Name == key)
		{
			key_index = i;
		}
	}

	if (key_index == -1)
	{
		INIKey _key{};
		_key.Name = key;
		Keys.push_back(_key);
		key_index = Keys.size() - 1;
	}

	return Keys[key_index].Value;
}

INIFile INIFile::operator+(const INIFile file)
{
	for (uint64_t kat_f{ 0 }; kat_f < file._categories.size(); kat_f++)
	{
		uint64_t is_already_inside = uint64_t(-1);
		for (uint64_t kat{ 0 }; kat < _categories.size(); kat++)
		{
			if (_categories[kat].Name == file._categories[kat_f].Name)
			{
				is_already_inside = kat;
			}
		}

		if (is_already_inside != uint64_t(-1))
		{
			for (uint64_t key_f{ 0 }; key_f < file._categories[kat_f].Keys.size(); key_f++)
			{
				std::string name = file._categories[kat_f].Name;
				uint64_t is_inside = uint64_t(-1);
				for (uint64_t key{ 0 }; key < _categories[is_already_inside].Keys.size(); key++)
				{
					if (file._categories[kat_f].Keys[key_f].Name == _categories[is_already_inside].Keys[key].Name)
					{
						is_inside = key;
					}
				}

				if (is_inside == uint64_t(-1))
				{
					_categories[is_already_inside].Keys.push_back(file._categories[kat_f].Keys[key_f]);
				}
			}
		}
		else
		{
			// Add category if it does not already exist.
			_categories.push_back(file._categories[kat_f]);
		}
	}
	return *this;
}

INIFile INIFile::operator+=(const INIFile file)
{
	return (*this + file);
}

void INIFile::ClearEmptyItems()
{
	for (uint64_t cat{ 0 }; cat < _categories.size(); cat++)
	{
		for (uint64_t key{ 0 }; key < _categories[cat].Keys.size(); key++)
		{
			bool erase_key = false;
			if (_categories[cat].Keys[key].Value.empty())
			{
				erase_key = true;
			}
			else if (_categories[cat].Keys[key].Name.empty())
			{
				erase_key = true;
			}
			// Remove key if it has an empty name or value.
			if (erase_key)
			{
				_categories[cat].Keys.erase(_categories[cat].Keys.begin() + key);
			}
		}

		bool erase_cat = false;
		if (_categories[cat].Keys.empty())
		{
			erase_cat = true;
		}
		else if (_categories[cat].Name.empty())
		{
			erase_cat = true;
		}
		// Remove category if it has an empty name or vector with keys.
		if (erase_cat)
		{
			_categories.erase(_categories.begin() + cat);
		}
	}
}

void INIFile::Clear()
{
	for (auto& cat : _categories)
	{
		cat.Keys.clear();
	}
	_categories.clear();
}
