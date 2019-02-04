#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>

int IsNote(const std::string &str)
{
    if (str.empty())
        return -1;

    for (std::string::const_iterator i = str.begin(); i < str.end(); i++)
    {
        if (*i == ' ')
            continue;

        if (*i == '#')
            return 1;
        else
            return 0;
    }
    return 2;
}

int main(int argc, char *argv[])
{
    #if defined(_WIN32)
    std::string file = "C:\\Windows\\System32\\drivers\\etc\\hosts";
    #elif defined(__linux__) || defined(__unix__ )
    std::string file = "/etc/hosts";
    #else
    #error "Unknown compiler"
    #endif

    std::string content;

    std::ifstream ifs(file.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!ifs.is_open())
    {
        std::cerr << "File '" << file.c_str() << "' not found!\n";
        return 1;
    }
    else
    {
        ifs.seekg(0, std::ios::end);
        int size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        char *buff = new (std::nothrow) char[size + 1];
        if (!buff)
        {
            std::cerr << "Failed to allocate memory\n";
            ifs.close();
            return 1;
        }
        memset(buff, 0, size + 1);
        ifs.read(buff, size);

        content = buff;

        delete[] buff;
        ifs.close();
    }

    std::vector<std::string> lines;
    std::map<std::string, std::string> rules;

    size_t pos = -1;
    size_t last_pos = 0;

    std::string tmp;

    while ((pos = content.find('\n', pos + 1)) != std::string::npos)
    {
        if (pos - last_pos > 1)
        {
            tmp = content.substr(last_pos, pos - last_pos);

            tmp.erase(std::remove(tmp.begin(), tmp.end(), '\r'), tmp.end());
            std::replace(tmp.begin(), tmp.end(), '\t', ' ');

            if (IsNote(tmp) == 0)
                lines.push_back(tmp);
        }
        last_pos = pos + 1;
    }
    if (last_pos != content.size())
    {
        if (content.size() - last_pos > 1)
        {
            tmp = content.substr(last_pos, content.size() - last_pos);

            tmp.erase(std::remove(tmp.begin(), tmp.end(), '\r'), tmp.end());
            std::replace(tmp.begin(), tmp.end(), '\t', ' ');

            if (IsNote(tmp) == 0)
                lines.push_back(tmp);
        }
    }

    for (std::vector<std::string>::const_iterator i = lines.begin(); i < lines.end(); i++)
    {
        std::vector<std::string> words;
        std::istringstream iss(*i);
        while (iss >> tmp)
            words.push_back(tmp);

        if (words.size() >= 2 &&
            words[0].find('#', 0) == std::string::npos &&
            words[1][0] != '#')
        {
            for (std::vector<std::string>::const_iterator j = words.begin() + 1; j < words.end(); j++)
            {
                pos = (*j).find('#', 0);
                if (pos == std::string::npos)
                {
                    rules.insert(std::make_pair(*j, words[0]));
                }
                else
                {
                    rules.insert(std::make_pair((*j).substr(0, pos), words[0]));
                    break;
                }
            }
        }
    }

    std::cout << "Rules Counter: " << rules.size() << "\n";

    if (rules.size() > 0)
    {
        std::cout << "Rules:\n";

        int counter = 1;
        for (std::map<std::string, std::string>::const_iterator i = rules.begin(); i != rules.end(); i++)
        {
            std::cout << "#" << (counter++) << " [" << i->first << "] -> [" << i->second << "]\n";
        }
    }

    return 0;
}
