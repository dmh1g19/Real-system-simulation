import subprocess
import matplotlib.pyplot as plt
import pandas as pd

binary_path = "/home/test/Desktop/real-time/cw1/comp3215-lab1-2023/bin/benchmark_real_time_database"

db_type = 'efficient'
num_entries = 100
num_threads = 1
num_accesses_values = list(range(0, 1000001, 10000))

#Run program to perform tests for 1.3 - Efficient databse vs naive database where number of threads = 1
for num_accesses in num_accesses_values:
    command = [binary_path, db_type, str(num_accesses), str(num_entries), str(num_threads)]
    subprocess.run(command)

#naive data
df_naive = pd.read_csv("1.2-graph_data_naive.csv")
accesses_data_naive = df_naive["accesses"].tolist()
wall_time_data_naive = df_naive["time"].tolist()

#efficient data
df_efficient = pd.read_csv("1.2-graph_data_efficient.csv")
accesses_data_efficient = df_efficient["accesses"].tolist()
wall_time_data_efficient = df_efficient["time"].tolist()

#aymptotic line for O(n)
asymptotic_x_values = range(0, 1000001, 10000)
asymptotic_y_values = [x / 1000000 for x in asymptotic_x_values]

plt.figure(figsize=(8, 6))
plt.plot(accesses_data_naive, wall_time_data_naive, marker='o', linestyle='-', color='b', markersize=4, label="Naive O(nm)")
plt.plot(accesses_data_efficient, wall_time_data_efficient, marker='o', linestyle='-', color='r', markersize=4, label="Efficient O(n)")
plt.plot(asymptotic_x_values, asymptotic_y_values, linestyle='--', color='g', label="Asymptotic O(n)")
plt.title("accesses versus walltime on linux x86-64 i5-2520m cpu @ 2.50ghz 4 cores - efficient", fontsize=12)
plt.xlabel("Number of Accesses (0.1 = 100000)")
plt.ylabel("Wall Time (seconds)")
plt.grid(True)
plt.legend()
plt.savefig("1.3-efficient_db_access_scaling.pdf", format="pdf", bbox_inches="tight")
#plt.show()