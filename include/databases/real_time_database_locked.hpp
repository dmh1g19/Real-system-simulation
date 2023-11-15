#ifndef real_time_database_locked_hpp
#define real_time_database_locked_hpp

#include "real_time_database_naive.hpp"

/* Everything is the same as the naive implementation, simply added a mutex lock
at the start of each operation to ensure only one thread accesses the critical region at any time.
Processig time is still slow at O(Number of accesses * number of keys) */

class RealTimeDatabaseLocked : public RealTimeDatabase
{
private:
    struct entry{
        std::string name;
        std::vector<char> data;
    };

    std::vector<entry> entries;
    std::mutex m;

public:
    RealTimeDatabaseLocked() {

    }

    std::string GetName() const override { 
        return "locked";
    }

    bool IsThreadSafe() const override {
        return true; 
    }

    intptr_t Register(const char *name, size_t size) override {
        std::unique_lock<std::mutex> lock(m);
        
        for(auto &e : entries){
            if(e.name==name){
                assert(e.data.size() == size);
                return 0;
            }
        }

        entries.push_back({ name, std::vector<char>(size, 0) });

        return 0;
    }

    void Read(const char *name, intptr_t token, size_t size, void *data) override {    
        std::unique_lock<std::mutex> lock(m);

        for(auto &e : entries){
            if(e.name==name){
                assert(e.data.size()==size);
                memcpy(data, &e.data[0], size);
                return;
            }
        }
        
        memset(data, 0, size);
    }

    void Write(const char *name, intptr_t token, size_t size, const void *data) override {
        std::unique_lock<std::mutex> lock(m);

        for(auto &e : entries){
            if(e.name==name){
                assert(e.data.size()==size);
                memcpy(&e.data[0], data, size);
                return;
            }
        }

        entries.push_back({ name, std::vector<char>(size, 0) });
        memcpy(&entries.back().data[0], data, size);
    }
};

#endif
