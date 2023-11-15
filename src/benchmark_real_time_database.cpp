#include "real_time_database_factory.hpp"

#include <unordered_set>
#include <atomic>
#include <random>
#include <iostream>
#include <fstream>
#include <string>

#include "time_helpers.hpp"

/* Generate a unique name in a thread-safe way.
*/
std::string make_unique_name()
{
    static std::atomic<uint64_t> names_seed; // Rely on static zero initialisation

    uint64_t seed=names_seed.fetch_add(19937);

    return "key_"+std::to_string(seed);
}

/*
Format of strings is:
x[0] = fill;
x[1] = (len / 10) + '0';
x[2] = (len % 10) + '0'
X[3] = key_token + 'a';
x[4..len-1] = fill;

Where fill is a random value in A..Z.
len is derived from the key.
key_token is derived from the key and is in a..z
*/

std::string make_payload_string(unsigned len, char key_token, char fill)
{
    assert(len >= 4);

    assert(4 <= len && len <= 99);
    assert('a' <= key_token && key_token <= 'z');
    assert('A' <= fill && fill <= 'Z');

    std::string res;
    
    res.resize(len);
    res[0]=fill;
    res[1]=(len/10)+'0';
    res[2]=(len%10)+'0';
    res[3]=key_token;

    std::fill(res.begin()+4, res.end(), fill);

    return res;
}

template<class TRng>
std::vector<uint8_t> create_random_value_with_checksum(const std::string &key, TRng &&rng)
{
    auto key_hash=std::hash<std::string>()(key);
    
    size_t len = 4 + (key_hash % (99-4)); // Value lengths in range [4,99) bytes
    char key_token = ( (key_hash>>2) % 26)+'a';
    char fill='A'+((rng())%26);

    std::string value=make_payload_string(len, key_token, fill);
    
    return std::vector<uint8_t>(value.begin(), value.end() );
}

template<class TRng>
void update_random_value_with_checksum(const std::string &key, TRng &&rng, std::vector<uint8_t> &value)
{
    char fill='A'+(rng()%26);

    value[0]=fill;

    std::fill(value.begin()+4, value.end(), fill);
}


bool validate_checksum(const std::string &key, const std::vector<uint8_t> &data)
{
    if(data.size() < 4 ){
        return false;
    }

    auto key_hash=std::hash<std::string>()(key);
    size_t len = 4 + (key_hash % (99-4)); // Value lengths in range [4,99) bytes
    char key_token = ( (key_hash>>2) % 26)+'a';

    unsigned got_len = (data[1]-'0')*10 + (data[2]-'0');
    if(got_len != len || got_len!=data.size()){
        return false;
    }
    if(data[3]!=key_token){
        return false;
    }

    auto fill=data[0];
    for(unsigned i=4; i<len; i++){
        if(data[i]!=fill){
            return false;
        }
    }

    return true;
}


struct test_parameters
{
    unsigned numAccess = 1000000; // Number of accesses in total, across all threads
    unsigned numEntries = 100;
    unsigned numThreads = 1;
    double probWrite = 0.5;    // Probability that access is a write
    uint64_t seed = 1;

    static void write_header(FILE *dst)
    {
        fprintf(dst, "Accesses,Entries,Threads");
    }

    void write(FILE *dst) const
    {
        fprintf(dst, "%u,%u,%u", numAccess, numEntries, numThreads);
    }

    void write_key_value(FILE *dst) const
    {
        fprintf(dst, "NumAccess=%u,NumEntries=%u,NumThreads=%u", numAccess, numEntries, numThreads);
    }
};

struct test_results
{
    test_parameters parameters;
    std::string name;  // Name of the implementation
    unsigned numChecksumFailures;
    unsigned hardwareConcurrency; // Result of std::hardware_concurrency
    double wallTime;    // Wall-clock time to execute benchmark

    static void write_header(FILE *dst)
    {
        fprintf(dst, "Implementation,");
        test_parameters::write_header(dst);
        fprintf(dst, ",CheckSumFailures,HWThreads,WallTime");
    }

    void write(FILE *dst) const
    {
        fprintf(dst,"%s,",name.c_str());
        parameters.write(dst);
        fprintf(dst, ",%u,%u,%g", numChecksumFailures, hardwareConcurrency, wallTime);
    }
};

test_results test_workload( RealTimeDatabase *db, const test_parameters &parameters)
{
    struct entry{
        std::string name;
        size_t size;
        intptr_t token;
    };

    std::vector<entry> entries;

    std::mt19937_64 rng(parameters.seed);
    std::uniform_real_distribution<> udist;

    fprintf(stderr, "Creating keys...");
    for(unsigned i=0; i<parameters.numEntries; i++){
        std::string name=make_unique_name();
        std::vector<uint8_t> value=create_random_value_with_checksum(name, rng);
        intptr_t token=db->Register(name.c_str(), value.size());
        entries.push_back({ name, value.size(), token});
        db->Write(name.c_str(), token, value.size(), &value[0]);
    }
    fprintf(stderr, "done\n");

    std::atomic<unsigned> checksum_failures;
    checksum_failures.store(0);

    auto mutator = [&](unsigned thread_offset)
    {
        std::mt19937_64 rng(parameters.seed + thread_offset);

        std::vector<uint8_t> tmp;
        for(unsigned i=0; i<parameters.numAccess/parameters.numThreads; i++){
            auto &e = entries[ rng() % parameters.numEntries ];
            
            tmp.resize(e.size);
            db->Read( e.name.c_str(), e.token, tmp.size(), &tmp[0] );
            
            if(!validate_checksum(e.name, tmp)){
                checksum_failures.fetch_add(1, std::memory_order_relaxed);
            }

            if(udist(rng) < parameters.probWrite){
                update_random_value_with_checksum(e.name, rng, tmp);
                db->Write( e.name.c_str(), e.token, tmp.size(), &tmp[0] );
            }
        }
    };


    std::vector<std::thread> threads;

    double startTime=now();

    for(unsigned i=0; i<parameters.numThreads; i++){
        auto mutator_i = [&,i](){ mutator(i); };
        threads.push_back(std::thread(mutator_i));
    }

    for(unsigned i=0; i<parameters.numThreads; i++){
        threads[i].join();
    }

    double endTime=now();

    test_results res;
    res.parameters=parameters;
    res.name=db->GetName();
    res.hardwareConcurrency=std::thread::hardware_concurrency();
    res.numChecksumFailures=checksum_failures.load();
    res.wallTime=endTime-startTime;

    //Export results - access and wall time
    //std::ofstream csvFile("graph_data.csv", std::ios::app);
    //csvFile << res.parameters.numAccess << "," << res.wallTime << std::endl;
    //csvFile.close();

    //Export results - threads and wall time
    std::ofstream csvFile("graph_data.csv", std::ios::app | std::ios::out);
    csvFile << res.parameters.numThreads << "," << res.wallTime << std::endl;
    csvFile.close();

    fprintf(stderr, "Found %u failures in %u updates across %u threads.\n", res.numChecksumFailures, parameters.numAccess, parameters.numThreads);

    return res;
}

int main(int argc, char *argv[])
{
    test_parameters params;

    std::string name="naive";
    if(argc>1){
        name=argv[1];
    }
    if(argc>2){
        params.numAccess=std::stoi(argv[2]);
    }
    if(argc>3){
        params.numEntries=std::stoi(argv[3]);
    }
    if(argc>4){
        params.numThreads=std::stoi(argv[4]);
    }
    params.probWrite=0.5;
    params.seed=time(0);

    fprintf(stderr, "Name=%s, ", name.c_str());
    params.write_key_value(stderr);
    fputs("\n", stderr);

    auto db=create_real_time_database(name);


    if(params.numThreads>1 && !db->IsThreadSafe()){
        fprintf(stderr, "Refusing to run with numThreads>1 as database %s has IsThreadSafe()==0\n", db->GetName().c_str());
        exit(1);
    }

    auto res=test_workload(db.get(), params);

    test_results::write_header(stdout);
    fputs("\n", stdout);
    res.write(stdout);
    fputs("\n", stdout);
}
