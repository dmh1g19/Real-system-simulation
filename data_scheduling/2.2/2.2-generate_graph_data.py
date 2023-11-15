import subprocess
import matplotlib.pyplot as plt
import pandas as pd

df_efficient = pd.read_csv("data.csv")
error_max = df_efficient["error_max"].tolist()
std_dev = df_efficient["error_stddev"].tolist()

step_size = 0.001
interference_point = [round(i * step_size, 3) for i in range(int(max(error_max) / step_size) + 2)]

plt.figure(figsize=(8, 6))
plt.plot(range(0, 10), std_dev, marker='o', linestyle='-', color='b', markersize=4, label="std deviation")
plt.plot(range(0, 10), error_max, marker='o', linestyle='-', color='r', markersize=4, label="max error")
plt.plot([4] * len(interference_point), interference_point, marker='', linestyle='--', color='g', markersize=4, label="inteference point")
plt.title("Batch max err & batch std deviation over 10 seconds with intervention - linux x86-64 i5-2520m cpu @ 2.50ghz 4 cores", fontsize=12)
plt.xlabel("Seconds")
plt.ylabel("Error")
plt.grid(True)
plt.legend()
plt.text(0.5, -0.12, "Jitter introduced by opening a browser mid execution", ha='center', va='center', transform=plt.gca().transAxes)
plt.savefig("2.2-queue_task1_high_jitter.pdf", format="pdf", bbox_inches="tight")
plt.show()