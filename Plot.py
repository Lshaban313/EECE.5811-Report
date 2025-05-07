#!/usr/bin/env python3
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# ─── CONFIGURATION ────────────────────────────────────────────────────────
# where  C++ wrote the CSVs:
CSV_DIR = r""

# modes and workloads:
MODES = {'Default': 'default', 'RT': 'rt'}
WORKLOADS = ['light', 'normal', 'moderate', 'high', 'extreme']
CSV_TMPL   = 'metrics_{mode}_{wl}.csv'

# which workload to use for timeseries/hist/CDF/box:
SELECTED_WL = 'normal'
# ────────────────────────────────────────────────────────────────────────────

os.chdir(CSV_DIR)

# ─── 1) LOAD data ──────────────────────────────────────────────────────────
# DataFrames per mode for the SELECTED_WL
df_sel = {}
for label, mode_key in MODES.items():
    fn = CSV_TMPL.format(mode=mode_key, wl=SELECTED_WL)
    if not os.path.exists(fn):
        raise FileNotFoundError(f"Missing CSV: {fn}")
    df = pd.read_csv(fn)
    df_sel[label] = df

# Steady-state (exclude cycle 0)
steady_sel = {
    label: df.loc[df['cycle'] != 0, 'latency_us'].values
    for label, df in df_sel.items()
}


# ─── 2) PLOT time series ────────────────────────────────────────────────────
plt.figure(figsize=(10, 5))
for label, df in df_sel.items():
    plt.plot(df['cycle'], df['latency_us'],
             marker='o', markersize=3, linewidth=1, label=label)

for label, df in df_sel.items():
    init_lat = df.loc[df['cycle'] == 0, 'latency_us'].iloc[0]
    plt.scatter(0, init_lat, s=100, zorder=5)
    plt.text(2, init_lat,
             f"{label} init\n{init_lat} µs",
             va='bottom')

plt.title(f"Control-Loop Latency over Cycles ({SELECTED_WL.capitalize()})")
plt.xlabel("Cycle")
plt.ylabel("Latency (µs)")
plt.grid(True, linestyle='--', alpha=0.5)
plt.legend()
plt.tight_layout()
plt.savefig(f"latency_timeseries_{SELECTED_WL}.png", dpi=150)


# ─── 3) PLOT histogram ─────────────────────────────────────────────────────
plt.figure(figsize=(8, 5))
bins = 50
for label, arr in steady_sel.items():
    plt.hist(arr, bins=bins, alpha=0.6, label=label)
plt.title(f"Latency Distribution ({SELECTED_WL.capitalize()})")
plt.xlabel("Latency (µs)")
plt.ylabel("Count")
plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.legend()
plt.tight_layout()
plt.savefig(f"latency_histogram_{SELECTED_WL}.png", dpi=150)


# ─── 4) PLOT CDF ───────────────────────────────────────────────────────────
plt.figure(figsize=(8, 5))
for label, arr in steady_sel.items():
    sorted_arr = np.sort(arr)
    cdf = np.arange(1, len(sorted_arr)+1) / len(sorted_arr)
    plt.step(sorted_arr, cdf, where='post', label=label)
plt.title(f"Latency CDF ({SELECTED_WL.capitalize()})")
plt.xlabel("Latency (µs)")
plt.ylabel("Cumulative Probability")
plt.grid(True, linestyle='--', alpha=0.5)
plt.legend()
plt.tight_layout()
plt.savefig(f"latency_cdf_{SELECTED_WL}.png", dpi=150)


# ─── 5) PLOT boxplot ───────────────────────────────────────────────────────
plt.figure(figsize=(6, 6))
data_to_box = [steady_sel[label] for label in MODES]
plt.boxplot(data_to_box, labels=list(MODES.keys()), showfliers=True)
plt.title(f"Latency Boxplot ({SELECTED_WL.capitalize()})")
plt.ylabel("Latency (µs)")
plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig(f"latency_boxplot_{SELECTED_WL}.png", dpi=150)


# ─── 6) BAR CHARTS (all workloads) ─────────────────────────────────────────
# pre calculate means & stddevs without cycle 0
means   = {lbl: [] for lbl in MODES}
jitters = {lbl: [] for lbl in MODES}
for wl in WORKLOADS:
    for lbl, mode_k in MODES.items():
        fn = CSV_TMPL.format(mode=mode_k, wl=wl)
        df = pd.read_csv(fn)
        arr = df.loc[df['cycle'] != 0, 'latency_us']
        means  [lbl].append(arr.mean())
        jitters[lbl].append(arr.std())

x = np.arange(len(WORKLOADS))
w = 0.35

# 6a) mean latency ± stddev
plt.figure(figsize=(8, 6))
plt.bar(x - w/2, means['Default'], width=w,
        yerr=jitters['Default'], capsize=4, label='Default')
plt.bar(x + w/2, means['RT'],      width=w,
        yerr=jitters['RT'],      capsize=4, label='RT')
plt.xticks(x, [wl.capitalize() for wl in WORKLOADS], rotation=15, ha='right')
plt.ylabel('Mean Latency (µs)')
plt.title('Mean Control-Loop Latency by Workload & Scheduler')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.legend()
plt.tight_layout()
plt.savefig('mean_latency.png', dpi=150)

# 6b) jitter (stddev) bar chart
plt.figure(figsize=(8, 6))
plt.bar(x - w/2, jitters['Default'], width=w, label='Default')
plt.bar(x + w/2, jitters['RT'],      width=w, label='RT')
plt.xticks(x, [wl.capitalize() for wl in WORKLOADS], rotation=15, ha='right')
plt.ylabel('Latency Std Dev (µs)')
plt.title('Latency Jitter by Workload & Scheduler')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.legend()
plt.tight_layout()
plt.savefig('latency_jitter.png', dpi=150)

# ─── FINISH ─────────────────────────────────────────────────────────────────
plt.show()
