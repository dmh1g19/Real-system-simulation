import subprocess
import matplotlib.pyplot as plt
import pandas as pd
import time

binary_path = "/home/test/Desktop/real-time/cw1/comp3215-lab1-2023/bin/benchmark_real_time_database"
#
#db_type = 'scalable'
#num_accesses = 1000000
#num_entries = 100
num_threads_values = list(range(1, 30, 1))
#
#for num_threads in num_threads_values:
#    command = [binary_path, db_type, str(num_accesses), str(num_entries), str(num_threads)]
#    subprocess.run(command)
#    time.sleep(0.2)

df_1 = pd.read_csv("graph_data_short.csv")
accesses_data_1 = df_1["threads"].tolist()
wall_time_data_1 = df_1["time"].tolist()

num_threads_inverse = [0.5 / (t) for t in num_threads_values]

plt.figure(figsize=(8, 6))
plt.plot(num_threads_values, num_threads_inverse, linestyle='--', color='g', label="O(1/num_threads)")
plt.plot(accesses_data_1, wall_time_data_1, marker='o', linestyle='-', color='b', markersize=4, label="threads")
plt.title("threads versus walltime on linux x86-64 i5-2520m cpu @ 2.50ghz 4 cores - scalable", fontsize=12)
plt.xlabel("Number of threads")
plt.ylabel("Wall Time (seconds)")
plt.grid(True)
plt.legend()
plt.savefig("1.7-scalable_db_thread_scaling.pdf", format="pdf", bbox_inches="tight")
plt.show()