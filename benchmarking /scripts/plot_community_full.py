import pandas as pd
import matplotlib.pyplot as plt
import os

df = pd.read_csv("results/community_results.csv")

algos = df["Algorithm"].unique()
graphs = ["sparse", "dense", "scale_free", "small_world"]

# ========== 1️⃣  RUNTIME ==========
pivot_time = df.pivot(index="Graph", columns="Algorithm", values="Time_ms").loc[graphs]
pivot_time.plot(kind="bar")
plt.ylabel("Runtime (ms)")
plt.title("Community Detection Runtime Comparison")
plt.xticks(rotation=0)
plt.tight_layout()
os.makedirs("plots", exist_ok=True)
plt.savefig("plots/community_runtime.png")
plt.close()

# ========== 2️⃣ MEMORY ==========
pivot_mem = df.pivot(index="Graph", columns="Algorithm", values="Memory_MB").loc[graphs]
pivot_mem.plot(kind="bar")
plt.ylabel("Memory Usage (MB)")
plt.title("Community Detection Memory Usage")
plt.xticks(rotation=0)
plt.tight_layout()
plt.savefig("plots/community_memory.png")
plt.close()

# ========== 3️⃣ COMMUNITIES ==========
pivot_comm = df.pivot(index="Graph", columns="Algorithm", values="Communities").loc[graphs]
pivot_comm.plot(kind="bar")
plt.ylabel("Number of Communities Found")
plt.title("Communities Detected by Algorithm")
plt.xticks(rotation=0)
plt.tight_layout()
plt.savefig("plots/community_count.png")
plt.close()

# ========== 4️⃣ MODULARITY ==========
pivot_mod = df.pivot(index="Graph", columns="Algorithm", values="Modularity").loc[graphs]
pivot_mod.plot(kind="bar")
plt.ylabel("Modularity Score")
plt.title("Modularity of Community Structure")
plt.xticks(rotation=0)
plt.tight_layout()
plt.savefig("plots/community_modularity.png")
plt.close()

print("\n🔥 SAVED:")
print("plots/community_runtime.png")
print("plots/community_memory.png")
print("plots/community_count.png")
print("plots/community_modularity.png\n")

