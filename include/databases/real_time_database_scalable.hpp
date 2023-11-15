#ifndef real_time_database_scalable_hpp
#define real_time_database_scalable_hpp

#include "real_time_database.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <atomic>

/* We achieve a scalable solution with the use of fine grained locks for each key, 
quite efficient and scalable, depending on CPU */

class RealTimeDatabaseScalable : public RealTimeDatabase {
private:
    std::unordered_map<std::string, std::vector<char>> entries;
    std::unordered_map<std::string, std::shared_mutex> locks;

public:
    std::string GetName() const override {
        return "scalable";
    }

    bool IsThreadSafe() const {
        return true;
    }

    intptr_t Register(const char* name, size_t size) override {
        std::unique_lock lock(locks[name]);

        auto it = entries.find(name);

        if (it != entries.end()) {
            return 0;
        }

        entries[name] = std::vector<char>(size, 0);

        return entries[name][0];
    }

    void Read(const char* name, intptr_t token, size_t size, void* data) override {
        std::shared_lock lock(locks[name]);

        if (token != 0) {
            memcpy(data, &token, size);
        } else {
            auto search = entries.find(name);
            if (search != entries.end()) {
                memcpy(data, &search->second[0], size);
            }
        }
    }

    void Write(const char* name, intptr_t token, size_t size, const void* data) override {
        std::unique_lock lock(locks[name]); 

        if (token != 0) {
            memcpy(&token, data, size);
        } else {
            auto search = entries.find(name);
            if (search != entries.end()) {
                memcpy(&search->second[0], data, size);
            }
        }
    }
};

#endif
