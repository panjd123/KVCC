import os
import os.path as osp
import numpy as np
import seaborn as sns
import re
import pandas as pd
import matplotlib.pyplot as plt

baseline_log_file = "../logs/baseline_0628_191211.log"
sweep1_log_file = "../logs/sweep_0628_225009.log"
sweep2_log_file = "../logs/sweep_0628_205714.log"


def extract(file):
    with open(file, "r") as f:
        log = f.readlines()
        log = [t.split("=") for t in log]
        # strip
        log = [[t[0].strip(), t[1].strip()] for t in log]  # key=value

        def extract_values(log, s):
            return [t[1] for t in log if t[0] == s]

        columns = ["graph", "k", "kvcc time"]
        values = [extract_values(log, s) for s in columns]

        df = pd.DataFrame(np.array(values).T, columns=columns)
        df["time"] = df["kvcc time"].astype(float)
        df["time"] = round(df["time"], 2)
        df = df.groupby(["graph", "k"]).mean(numeric_only=True)
        return df


baseline_result = extract(baseline_log_file)
sweep1_result = extract(sweep1_log_file)
sweep2_result = extract(sweep2_log_file)
merged = pd.merge(
    baseline_result,
    sweep1_result,
    how="inner",
    on=["graph", "k"],
    suffixes=("_baseline", "_sweep1"),
)
merged = pd.merge(merged, sweep2_result, how="inner", on=["graph", "k"])
merged.rename(columns={"time": "time_sweep2"}, inplace=True)
merged.insert(
    2, "speedup_sweep1", round(merged["time_baseline"] / merged["time_sweep1"], 2)
)
merged.insert(
    4, "speedup_sweep2", round(merged["time_baseline"] / merged["time_sweep2"], 2)
)
# heatmap
print(merged)
fig, ax = plt.subplots(1, 2, figsize=(12, 6))
merged = merged.reset_index()
# share the same colorbar
vmin = min(merged["speedup_sweep1"].min(), merged["speedup_sweep2"].min())
vmax = max(merged["speedup_sweep1"].max(), merged["speedup_sweep2"].max())
sns.heatmap(
    merged.pivot("graph", "k", "speedup_sweep1"),
    ax=ax[0],
    vmin=vmin,
    vmax=vmax,
    annot=True,
    cbar=False,
    fmt=".2f",
    cmap="YlGnBu",
)
sns.heatmap(
    merged.pivot("graph", "k", "speedup_sweep2"),
    ax=ax[1],
    vmin=vmin,
    vmax=vmax,
    annot=True,
    cbar=False,
    fmt=".2f",
    cmap="YlGnBu",
)
ax[0].set_title("original sweep speedup")
ax[1].set_title("optimized sweep speedup")
plt.colorbar(ax[0].collections[0], ax=ax, location="right", pad=0.05)
plt.savefig("heatmap_sweep.png")
