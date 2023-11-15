#include "real_time_database_factory.hpp"
#include "tasks/compute_task.hpp"
#include "tasks/output_task.hpp"
#include "tasks/input_task.hpp"
#include <thread>

int main()
{
    auto db = create_real_time_database("locked");

    InputTask t0(db.get());
    ComputeTask t1(db.get());
    OutputTask t2(db.get());

    auto createTaskThread = [](auto task) {
        while(1) {
            wait_until(task.GetNextDeadline());
            task.Execute();
        }
    };

    std::thread th0(createTaskThread, t0);
    std::thread th1(createTaskThread, t1);
    std::thread th2(createTaskThread, t2);

    th0.join();
    th1.join();
    th2.join();

    return 0;
}
