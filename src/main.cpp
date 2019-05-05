#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <list>
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#endif

typedef struct RULE {
    std::string ip;
    std::string hostname;
} *LPRULE;

static int IsNote(const std::string &str)
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

#if defined(_WIN32)
static std::string GetHostsFileNameInWindows(std::string default_file_name, bool *bIsSucceded = NULL)
{
    if (bIsSucceded)
        *bIsSucceded = false;

    std::string hosts_file = default_file_name;

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters",
                     0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        char szBuffer[1024];
        memset(szBuffer, 0, 1024);
        DWORD dwBufferSize = sizeof(szBuffer);
        if (RegQueryValueEx(hKey, "DataBasePath", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
        {
            hosts_file = szBuffer;

            if (!ExpandEnvironmentStrings(hosts_file.c_str(), szBuffer, dwBufferSize))
            {
                hosts_file = default_file_name;
            }
            else
            {
                if (bIsSucceded)
                    *bIsSucceded = true;
                hosts_file = szBuffer;
                hosts_file += "\\hosts";
            }
        }
        RegCloseKey(hKey);
    }
    return hosts_file;
}
#endif

int main(int argc, char *argv[])
{
    #if defined(_WIN32)
    bool bSucceded;
    std::string file = GetHostsFileNameInWindows("C:\\Windows\\System32\\drivers\\etc\\hosts", &bSucceded);
    if (!bSucceded)
    {
        std::cout << "Warning! Could not read value from Windows registry. The path to the hosts file is assigned by default.\n";
    }
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
        std::cerr << "Error! File '" << file.c_str() << "' not found.\n";
        return 1;
    }
    else
    {
        ifs.seekg(0, std::ios::end);
        int size = static_cast<int>(ifs.tellg());
        ifs.seekg(0, std::ios::beg);

        char *buff = new (std::nothrow) char[size + 1];
        if (!buff)
        {
            std::cerr << "Error! Failed to allocate memory.\n";
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
