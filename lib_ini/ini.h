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
#ifndef _INI_H_
#define _INI_H_

#if _WIN32
#   if INI_DLL
#       if INI_DLL_EXPORT
#           define INI_API __declspec(dllexport)
#       else
#           define INI_API __declspec(dllimport)
#       endif
#   else
#       define INI_API
#   endif
#else
#   error No supported platform
#endif

#pragma warning(push)
#pragma warning(disable : 4251)

#include <string>
#include <vector>

class INI_API INIFile
{
private:
    struct INIKey
    {
        std::string Name = "";
        std::string Value = "";
    };
    struct INI_API INICat
    {
        std::string Name = "";
        std::vector<INIKey> Keys;

        std::string& operator[](const std::string key);
    };

public:
    enum ErrorCodes
    {
        Success = 0,
        Failed
    };

public:
    INIFile();
    INIFile(std::string filepath);
    ~INIFile();

    /*
    * Loads the categories and their keys from a .ini-file.
    * Opens the file and closes it after loading.
    *
    * \return 0 on success and 1 on failure
    *
    * \param filepath the path with filename
    */
    int LoadFile(std::string filepath);

    /*
    * Saves the categories and their keys into a .ini-file.
    * Opens the file and closes it after saving.
    * 
    * \return 0 on success and 1 on failure
    * 
    * \param filepath the path with filename
    */
    int SaveFile(std::string filepath = "");

    INICat& operator[](const std::string kat);
    INIFile operator+(const INIFile file);
    INIFile operator+=(const INIFile file);

    /*
    * Removes all keys and categories which haven't any value.
    */
    void ClearEmptyItems();

    /*
    * Removes all keys and categories.
    */
    void Clear();

private:
    std::string _filepath;
    std::vector<INICat> _categories;
};

#pragma warning(pop)

#endif // _INI_H_
