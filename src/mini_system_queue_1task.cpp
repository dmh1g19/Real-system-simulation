
#include "real_time_database_factory.hpp"
#include "tasks/output_task.hpp"
#include <chrono>

int main()
{
    auto db=create_real_time_database("naive");

    OutputTask out(db.get());

    //Get start time
    auto start = std::chrono::steady_clock::now();

    while(1){
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        if (elapsed.count() >= 10) {
            break; 
        }

        double tNextDeadline = out.GetNextDeadline();
        wait_until(tNextDeadline);
        out.Execute();
    }
}
