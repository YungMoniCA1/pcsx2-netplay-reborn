/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2016  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Settings.h"

#if defined(_WIN32)
#include <codecvt>
#endif
#include <fstream>
#include <locale>
#include <string>

Settings::Settings()
{
}

Settings::Settings(std::map<std::string, std::string> data)
    : m_data(data)
{
}

void Settings::TrimWhitespace(std::string &str) const
{
    // Leading whitespace
    str.erase(0, str.find_first_not_of(" \r\t"));
    // Trailing whitespace
    auto pos = str.find_last_not_of(" \r\t");
    if (pos != std::string::npos && pos != str.size() - 1)
        str.erase(pos + 1);
}

void Settings::Load(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return;

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        auto separator = line.find('=');
        if (separator == std::string::npos)
            continue;

        std::string key = line.substr(0, separator);
        // Trim leading and trailing whitespace
        TrimWhitespace(key);
        if (key.empty())
            continue;

        std::string value = line.substr(separator + 1);
        TrimWhitespace(value);

        Set(key, value);
    }
}

void Settings::Save(const std::string &filename) const
{
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open())
        return;

    for (const auto &pair : m_data)
        file << pair.first << '=' << pair.second << '\n';
}

void Settings::Set(std::string key, std::string value)
{
    m_data[key] = value;
}

bool Settings::Get(const std::string &key, std::string &data) const
{
    auto it = m_data.find(key);
    if (it == m_data.end())
        return false;

    data = it->second;
    return true;
}

#if defined(_WIN32)
void Settings::Set(std::string key, std::wstring value)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    m_data[key] = converter.to_bytes(value);
}

bool Settings::Get(const std::string &key, std::wstring &data) const
{
    auto it = m_data.find(key);
    if (it == m_data.end())
        return false;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    data = converter.from_bytes(it->second);
    return true;
}
#endif
