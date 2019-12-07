#include "config.h"
#include <fstream>

namespace mvc
{
    template <typename T>
    static
    T get(String const& key, Json const& j)
    {
        try
        {
            auto const& item = j.at(key);
            return item.get<T>();
        }
        catch (Json::exception& e)
        {
            throw std::runtime_error("Failed to read config key '" + key + "': " + e.what());
        }
    }

    static
    Json load_json_from_file(String const& path)
    {
        std::ifstream file(path);

        if (!file)
        {
            throw std::runtime_error("Could not open file '" + path + "'");
        }

        Json j;
        file >> j;
        return j;
    }

    static
    void load_indirect_keys(Json& j)
    {
        Json::iterator const load_it = j.find("load_from_files");

        if (load_it == j.end())
        {
            return;
        }

        for (Json::iterator it = load_it->begin(); it != load_it->end(); ++it)
        {
            j[it.key()] = load_json_from_file(it.value());
        }
    }

    Config::Config(ConfigPath const& path)
    {
        try
        {
            _json = load_json_from_file(path.val());
            load_indirect_keys(_json);
        }
        catch (std::exception& e)
        {
            throw std::runtime_error(
                "Failed to load config '" + path.val() + "': " + e.what()
                );
        }
    }
    
    int Config::get_int(String const& key) const
    {
        return get<int>(key, _json);
    }

    String Config::get_string(String const& key) const
    {
        return get<String>(key, _json);
    }
}
