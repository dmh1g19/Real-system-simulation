import subprocess
import matplotlib.pyplot as plt
import pandas as pd

df_efficient = pd.read_csv("data.csv")
volume = df_efficient["Volume"].tolist()
error_mean = df_efficient["error_mean"].tolist()
error_max = df_efficient["error_max"].tolist()

step_size = 0.001
interference_point = [round(i * step_size, 3) for i in range(int(max(error_max) / step_size) + 2)]

plt.figure(figsize=(8, 6))
plt.plot(volume, error_mean, marker='o', linestyle='-', color='b', markersize=4, label="error mean")
plt.plot(volume, error_max, marker='o', linestyle='-', color='r', markersize=4, label="error max")
plt.plot([190] * len(interference_point), interference_point, marker='', linestyle='--', color='g', markersize=4, label="Phase change")
plt.title("Volume against the batch mean error and batch worst-case error for a stream of + characters - linux x86-64 i5-2520m cpu @ 2.50ghz 4 cores", fontsize=12)
plt.xlabel("Volume")
plt.ylabel("Error")
plt.grid(True)
plt.legend()
plt.text(0.5, -0.12, "System is missing deadlines as it cannot keep up with the volume, causing massive error increase", ha='center', va='center', transform=plt.gca().transAxes)
plt.savefig("2.3-queue_volume_versus_latency.pdf", format="pdf", bbox_inches="tight")
plt.show()