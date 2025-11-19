import pandas as pd
import matplotlib.pyplot as plt
import os

# Load CSV
df = pd.read_csv("results/centrality_results.csv")

# Ensure output folder exists
os.makedirs("plots", exist_ok=True)

# ========== 📌 1. RUNTIME PLOT ==========
plt.figure(figsize=(10,5))
for algo in df["Algorithm"].unique():
    subset = df[df["Algorithm"] == algo]
    plt.plot(subset["Graph"], subset["Time_ms"], marker="o", label=algo)

plt.ylabel("Time (ms)")
plt.title("Runtime of Centrality Algorithms Across Graph Types")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("plots/runtime.png")
plt.close()

# LOG SCALE VERSION (IMPORTANT)
plt.figure(figsize=(10,5))
for algo in df["Algorithm"].unique():
    subset = df[df["Algorithm"] == algo]
    plt.plot(subset["Graph"], subset["Time_ms"], marker="o", label=algo)

plt.yscale("log")
plt.ylabel("Time (ms, log-scale)")
plt.title("Runtime (Log Scale) – Centrality")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("plots/runtime_logscale.png")
plt.close()

# ========== 📌 2. MEMORY USAGE PLOT ==========
plt.figure(figsize=(10,5))
for algo in df["Algorithm"].unique():
    subset = df[df["Algorithm"] == algo]
    plt.plot(subset["Graph"], subset["Memory_MB"], marker="o", label=algo)

plt.ylabel("Memory (MB)")
plt.title("Memory Usage of Centrality Algorithms")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("plots/memory.png")
plt.close()

# ========== 📌 3. OPS COMPARISON PLOT ==========
plt.figure(figsize=(10,5))
for algo in df["Algorithm"].unique():
    subset = df[df["Algorithm"] == algo]
    plt.plot(subset["Graph"], subset["Ops"], marker="o", label=algo)

plt.yscale("log")
plt.ylabel("Estimated Operations (log scale)")
plt.title("Operation Count Comparison (Theoretical)")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("plots/ops.png")
plt.close()

print("\n🎉 ALL CENTRALITY PLOTS GENERATED inside /plots\n")
