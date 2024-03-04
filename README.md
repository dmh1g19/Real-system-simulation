# RT databases

The main topics covered include:

- Storing real-time entity state in a database
- Improving and optimising code to reduce dcomputer
- Running multiple tasks interacting through a real-time database
- Measuring and characterising jitter with multiple tasks

The implementation has been simplified and abstracted so that it can run in a standard laptop or desktop platform. This somewhat disconnects it from reality, but is necessary in order to avoid substantial overhead due to installing libraries or forcing exactly one platform.

Lets make a 'naive' database system for a realtime system no optimization and an optimized version. We can comapre the two and discus their differences.

# A naive vs efficient implementation

A simple and naive implementation of a database where **O(Num of accessess * Number of keys)**
and doesnt have any multi-threading capabilities.

This version simply uses a vector (much slower than something like a hashmap)

```C++
private:
struct entry {
    std::string name;
    std::vector<char> data;
};

std::vector<entry> entries;

public:
std::string GetName() const override { 
    return "naive";
}

bool IsThreadSafe() const override { 
    return false;
}

intptr_t Register(const char *name, size_t size) override {
    for(auto &e : entries){
        if(e.name==name){
            assert(e.data.size() == size);
            return 0;
        }
    }
    entries.push_back({ name, std::vector<char>(size, 0) });
    return 0;
}

void Read(const char *name, intptr_t token, size_t size, void *data) override
{
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
```

# Efficient version using a hashmap

Lets implement a more efficient version this implementation uses a hasmhmap **O(1) insertion** making it much faster at **O(Number of accesses)** but doesn't have any multi-threading capabilities.

```C++
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
```
# Naive vs efficient results

The naive implementation has WallTime = O(NumAccesses * NumKeys). So if you double the number of updates
execution time doubles (as expected), and if you double the number keys the execution time also doubles (which is bad).

Lets run the naive and efficient engine with 1000 accesses, 10 keys, and 1 thread.

By swapping to a hasmap we have increased the time efficiency substantially. Our system is currently not thread safe.

<img src="/img/1.3-efficient.png" width="600">

<br>

# Multi-threaded, scalable and efficient

Lets make it multi-threaded and efficient in relation to number of accesses on the database and number of threads running a process!

We achieve a scalable solution with the use of fine grained locks for each key, quite efficient and scalable, depending on CPU.

```C++
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
```
We can see from the graph below, the number of accesses is linear with wall time!

<img src="/img/1.5-scalabe.png" width="600">

We can also observe that WallTime is **O(1/NumThreads)** for number of threads, my computer is very bad - but with some better hardware this curve would look far better!

# Data summary of operations on the databses we've made
<br>

<img src="/img/1.7.png" width="600">
<br>

<img src="/img/2.1.png" width="600">
<br>

<img src="/img/2.2.png" width="600">
<br>

<img src="/img/2.3.png" width="600">
<br>

<img src="/img/2.5.png" width="600">
<br>
