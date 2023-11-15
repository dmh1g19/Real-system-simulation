import subprocess
import matplotlib.pyplot as plt
import pandas as pd

df_efficient = pd.read_csv("data.csv")
error_max = df_efficient["error_max"].tolist()
std_dev = df_efficient["error_stddev"].tolist()

plt.figure(figsize=(8, 6))
plt.plot(range(0, 10), std_dev, marker='o', linestyle='-', color='b', markersize=4, label="std deviation")
plt.plot(range(0, 10), error_max, marker='o', linestyle='-', color='r', markersize=4, label="max error")
plt.title("Batch max err & batch std deviation over 10 seconds - linux x86-64 i5-2520m cpu @ 2.50ghz 4 cores", fontsize=12)
plt.xlabel("Seconds")
plt.ylabel("Error")
plt.grid(True)
plt.legend()
plt.savefig("2.1-queue_task1_quiet.pdf", format="pdf", bbox_inches="tight")
plt.show()