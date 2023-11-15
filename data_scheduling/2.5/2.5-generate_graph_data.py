import subprocess
import matplotlib.pyplot as plt
import pandas as pd

df_efficient = pd.read_csv("data.csv")
volume = df_efficient["Volume"].tolist()
error_mean = df_efficient["error_mean"].tolist()
error_max = df_efficient["error_max"].tolist()

plt.figure(figsize=(8, 6))
plt.plot(volume, error_mean, marker='o', linestyle='-', color='b', markersize=4, label="error mean")
plt.plot(volume, error_max, marker='o', linestyle='-', color='r', markersize=4, label="error max")
plt.title("value of “volume” against the batch mean error and worst-case error for a stream of + keys (multi-threaded)", fontsize=12)
plt.xlabel("Volume")
plt.ylabel("Error")
plt.grid(True)
plt.legend()
plt.text(0.5, -0.12, "linux x86-64 i5-2520m cpu @ 2.50ghz 4 cores", ha='center', va='center', transform=plt.gca().transAxes)
plt.savefig("2.5-threads_volume_versus_latency.pdf", format="pdf", bbox_inches="tight")
plt.show()