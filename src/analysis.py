import os
import os.path as osp
import numpy as np
import seaborn as sns
import re
import pandas as pd
import matplotlib.pyplot as plt

baseline_log_file = "baseline_0628_191211.log"
sweep_log_file = "sweep_0628_192059.log"


def extract(file=sweep_log_file):
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
        df = df.groupby(["graph", "k"]).mean(numeric_only=True)
        return df


baseline_result = extract(baseline_log_file)
sweep_result = extract(sweep_log_file)
merged = pd.merge(
    baseline_result, sweep_result, on=["graph", "k"], suffixes=["_baseline", "_sweep"]
)
merged["improvement(%)"] = round(
    (merged["time_baseline"] - merged["time_sweep"]) / merged["time_baseline"] * 100,
    2,
)
sns.heatmap(merged["improvement(%)"].unstack(), annot=True)
plt.savefig("heatmap.png")
print(merged)
