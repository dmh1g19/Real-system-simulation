#ifndef real_time_database_factory_hpp
#define real_time_database_factory_hpp

#include "databases/real_time_database_naive.hpp"
#include "databases/real_time_database_locked.hpp"
#include "databases/real_time_database_efficient.hpp"
#include "databases/real_time_database_scalable.hpp"

inline std::unique_ptr<RealTimeDatabase> create_real_time_database(const std::string &name)
{
    if(name=="naive"){
        return std::unique_ptr<RealTimeDatabase>(new RealTimeDatabaseNaive());
    }else if(name=="locked"){
        return std::unique_ptr<RealTimeDatabase>(new RealTimeDatabaseLocked());
    }else if(name=="efficient"){
        return std::unique_ptr<RealTimeDatabase>(new RealTimeDatabaseEfficient());
    }else if(name=="scalable"){
        return std::unique_ptr<RealTimeDatabase>(new RealTimeDatabaseScalable());
    }else{
        throw std::runtime_error("No database implementation called '"+name+"'.");
        exit(1);
    }
}

#endif
