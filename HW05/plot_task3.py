"""Plot the Problem 3 vscale timings into task3.pdf.

Usage: python plot_task3.py

Reads task3_timings.txt written by run_task3.sh (one "threads_per_block n
time_ms" line per run) and plots the kernel time against n on log-log axes, one
line per block size. If gpu_info.txt (also from run_task3.sh) is present, the
GPU model goes into the subtitle. Saves task3.pdf plus a 300 dpi PNG copy for
assignment5.docx, and prints the timings with the effective memory bandwidth.
"""
import csv
import math
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
matplotlib.rcParams["pdf.fonttype"] = 42  # embed TrueType so PDF text stays selectable
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, FuncFormatter, NullLocator

# Categorical slots 1 and 2 of the reference palette, plus a marker per series.
STYLES = {512: ("#2a78d6", "o"), 16: ("#eb6834", "s")}
SURFACE = "#ffffff"
TEXT_PRIMARY = "#0b0b0b"
TEXT_SECONDARY = "#52514e"
GRID_COLOR = "#e1e0d9"
AXIS_COLOR = "#c3c2b7"
BYTES_PER_ELEMENT = 12  # read a[i] and b[i], write b[i]: three 4-byte floats


def read_timings(path):
    runs = defaultdict(dict)
    with open(path) as f:
        for line in f:
            fields = line.split()
            if len(fields) >= 3:
                runs[int(fields[0])][int(fields[1])] = float(fields[2])
    return runs


def gpu_name(path="gpu_info.txt"):
    if not Path(path).exists():
        return None
    with open(path, newline="") as f:
        rows = list(csv.reader(f))
    return rows[1][0].strip() if len(rows) > 1 and rows[1] else None


def main():
    runs = read_timings("task3_timings.txt")
    block_sizes = sorted(runs, reverse=True)
    sizes = sorted(set().union(*(runs[b] for b in block_sizes)))

    header = "".join(f"  {b:>4} thr/blk (ms)" for b in block_sizes)
    print(f"        n{header}")
    for n in sizes:
        row = "".join(f"  {runs[b].get(n, float('nan')):>17.4g}" for b in block_sizes)
        print(f"{n:>9}{row}")
    largest = sizes[-1]
    for b in block_sizes:
        if largest in runs[b]:
            gbps = BYTES_PER_ELEMENT * largest / (runs[b][largest] * 1e-3) / 1e9
            print(f"{b} threads per block, n = {largest}: {gbps:.0f} GB/s effective bandwidth")

    fig, ax = plt.subplots(figsize=(6.5, 4.2), facecolor=SURFACE)
    ax.set_facecolor(SURFACE)
    for b in block_sizes:
        color, marker = STYLES.get(b, ("#1baf7a", "^"))
        ns = sorted(runs[b])
        ax.plot(ns, [runs[b][n] for n in ns], color=color, marker=marker, markersize=5.5,
                linewidth=1.5, markeredgecolor=SURFACE, markeredgewidth=1.2,
                label=f"{b} threads per block", solid_joinstyle="round", zorder=3)

    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    low, high = int(math.log2(sizes[0])), int(math.log2(sizes[-1]))
    ax.xaxis.set_major_locator(FixedLocator([2**k for k in range(low, high + 1, 2)]))
    ax.xaxis.set_major_formatter(FuncFormatter(lambda v, _: f"$2^{{{round(math.log2(v))}}}$"))
    ax.xaxis.set_minor_locator(NullLocator())
    ax.yaxis.set_major_formatter(FuncFormatter(lambda v, _: f"{v:g}"))

    subtitle = f"GPU: {gpu_name()}" if gpu_name() else "one kernel launch per point"
    ax.set_title("Problem 3: vscale kernel time vs. n", color=TEXT_PRIMARY, loc="left", pad=20)
    ax.text(0, 1.02, subtitle, transform=ax.transAxes, ha="left", va="bottom", fontsize=9,
            color=TEXT_SECONDARY)
    ax.set_xlabel("n (array length)", color=TEXT_SECONDARY)
    ax.set_ylabel("kernel time (ms)", color=TEXT_SECONDARY)
    ax.grid(True, which="major", color=GRID_COLOR, linewidth=0.75)
    ax.set_axisbelow(True)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color(AXIS_COLOR)
    ax.tick_params(which="both", color=AXIS_COLOR, labelcolor=TEXT_SECONDARY)
    ax.legend(frameon=False, loc="upper left", labelcolor=TEXT_SECONDARY)

    fig.tight_layout()
    fig.savefig("task3.pdf")
    fig.savefig("task3.png", dpi=300)
    print("Saved task3.pdf and task3.png")


if __name__ == "__main__":
    main()
