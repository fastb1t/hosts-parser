#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <list>
#include <algorithm>

typedef struct RULE {
    std::string ip;
    std::string hostname;
} *LPRULE;

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

    std::cout << "Path to file: " << file << "\n";

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

    std::list<std::string> lines;
    std::vector<RULE> rules;

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

    for (std::list<std::string>::const_iterator i = lines.begin(); i != lines.end(); i++)
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
                if ((*j)[0] == '#')
                    break;

                RULE rule;
                rule.ip = words[0];

                if ((pos = (*j).find('#', 0)) == std::string::npos)
                {
                    rule.hostname = *j;
                }
                else
                {
                    rule.hostname = (*j).substr(0, pos);
                }
                rules.push_back(rule);

                if (pos != std::string::npos)
                    break;
            }
        }
    }

    std::cout << "Rules Counter: " << rules.size() << "\n";

    if (rules.size() > 0)
    {
        std::cout << "Rules:\n";

        int counter = 1;
        for (std::vector<RULE>::const_iterator i = rules.begin(); i < rules.end(); i++)
        {
            std::cout << "#" << (counter++) << " [" << (*i).hostname << "] -> [" << (*i).ip << "]\n";
        }
    }

    return 0;
}
