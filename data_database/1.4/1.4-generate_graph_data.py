import subprocess
import matplotlib.pyplot as plt
import pandas as pd

#binary_path = "/home/test/Desktop/real-time/cw1/comp3215-lab1-2023/bin/benchmark_real_time_database"
#
#db_type = 'scalable'
#num_accesses_values = list(range(0, 1000001, 10000))
#num_entries = 100
#num_threads = 2
#
#for num_accesses in num_accesses_values:
#    command = [binary_path, db_type, str(num_accesses), str(num_entries), str(num_threads)]
#    subprocess.run(command)

df = pd.read_csv("1.4-graph_data_scalable.csv")
accesses_data = df["accesses"].tolist()
wall_time_data = df["time"].tolist()

asymptotic_x_values = range(0, 1000001, 10000)
asymptotic_y_values = [x / 1000000 for x in asymptotic_x_values]

plt.figure(figsize=(8, 6))
plt.plot(accesses_data, wall_time_data, marker='o', linestyle='-', color='b', markersize=4, label="Scalable O(n)")
plt.plot(asymptotic_x_values, asymptotic_y_values, linestyle='--', color='g', label="Asymptotic O(n)")
plt.title("accesses versus walltime on linux x86-64 i5-2520m cpu @ 2.50ghz 4 cores - scalable", fontsize=12)
plt.xlabel("Number of Accesses (0.1 = 100000), threads = 2")
plt.ylabel("Wall Time (seconds)")
plt.grid(True)
plt.legend()
plt.savefig("1.5-scalable_db_update_scaling.pdf", format="pdf", bbox_inches="tight")
plt.show()