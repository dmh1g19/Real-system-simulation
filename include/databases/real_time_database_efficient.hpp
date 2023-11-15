#ifndef real_time_database_efficient_hpp
#define real_time_database_efficient_hpp

#include "real_time_database.hpp"
#include <unordered_map>

/* This implementation uses a hasmhmap (O(1) insertion) making it much faster at O(Number of accesses)
but doesn't have any multi-threading capabilities */ 

class RealTimeDatabaseEfficient : public RealTimeDatabase
{
private:
    std::unordered_map<std::string, std::vector<char>> entries;

public:
    std::string GetName() const override { 
        return "efficient";
    }

    bool IsThreadSafe() const override { 
        return false;
    }

    intptr_t Register(const char *name, size_t size) override {
        entries[name] = std::vector<char>(size, 0);
        return 0;
    }

    void Read(const char *name, intptr_t token, size_t size, void *data) override {
        auto it = entries.find(name);
        if (it != entries.end()) {
            assert(it->second.size() == size);
            memcpy(data, &it->second[0], size);
        } else {
            memset(data, 0, size);
        }
    }

    void Write(const char *name, intptr_t token, size_t size, const void *data) override {
        auto it = entries.find(name);
        if (it != entries.end()) {
            assert(it->second.size() == size);
            memcpy(&it->second[0], data, size);
        } else {
            entries[name] = std::vector<char>(size, 0);
            memcpy(&entries[name][0], data, size);
        }
    }
};

#endif
