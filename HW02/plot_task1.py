"""Plot the task1 scaling analysis (scan time vs. n) into task1.pdf.

Usage: python plot_task1.py [timings_file] [output_pdf]

The timings file holds one "n time_ms" pair per line, as written by
run_task1.sh. Defaults: task1_timings.txt -> task1.pdf.
"""
import math
import sys

import matplotlib

matplotlib.use("Agg")
matplotlib.rcParams["pdf.fonttype"] = 42  # embed TrueType so PDF text stays selectable
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, FuncFormatter, NullLocator

SERIES_COLOR = "#2a78d6"
SURFACE = "#ffffff"
TEXT_PRIMARY = "#0b0b0b"
TEXT_SECONDARY = "#52514e"
GRID_COLOR = "#e1e0d9"
AXIS_COLOR = "#c3c2b7"


def read_timings(path):
    sizes, times = [], []
    with open(path) as f:
        for line in f:
            if line.strip():
                n, time_ms = line.split()
                sizes.append(int(n))
                times.append(float(time_ms))
    return sizes, times


def format_ms(value, _position):
    return f"{value:,.0f}" if value >= 1 else f"{value:g}"


def main():
    timings_file = sys.argv[1] if len(sys.argv) > 1 else "task1_timings.txt"
    output_file = sys.argv[2] if len(sys.argv) > 2 else "task1.pdf"
    sizes, times = read_timings(timings_file)

    fig, ax = plt.subplots(figsize=(7, 4.5), facecolor=SURFACE)
    ax.set_facecolor(SURFACE)
    ax.plot(sizes, times, color=SERIES_COLOR, linewidth=1.5, marker="o", markersize=6,
            markeredgecolor=SURFACE, markeredgewidth=1.5,
            solid_joinstyle="round", solid_capstyle="round")

    # Log-log axes: linear scaling shows up as a straight line of slope 1.
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    low = math.floor(math.log2(min(sizes)))
    high = math.ceil(math.log2(max(sizes)))
    ax.xaxis.set_major_locator(FixedLocator([2**k for k in range(low, high + 1, 2)]))
    ax.xaxis.set_major_formatter(FuncFormatter(lambda v, _: f"$2^{{{round(math.log2(v))}}}$"))
    ax.xaxis.set_minor_locator(NullLocator())
    ax.yaxis.set_major_formatter(FuncFormatter(format_ms))
    ax.margins(x=0.04, y=0.12)

    ax.set_title("Task 1: inclusive scan scaling analysis", color=TEXT_PRIMARY, loc="left")
    ax.set_xlabel("n (number of elements)", color=TEXT_SECONDARY)
    ax.set_ylabel("scan time (ms)", color=TEXT_SECONDARY)

    # Label only the largest run.
    ax.annotate(f"{times[-1]:,.1f} ms", xy=(sizes[-1], times[-1]), xytext=(-10, 6),
                textcoords="offset points", ha="right", va="bottom", color=TEXT_SECONDARY)

    ax.grid(True, which="major", color=GRID_COLOR, linewidth=0.75)
    ax.set_axisbelow(True)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.spines["left"].set_color(AXIS_COLOR)
    ax.spines["bottom"].set_color(AXIS_COLOR)
    ax.tick_params(which="both", color=AXIS_COLOR, labelcolor=TEXT_SECONDARY)

    fig.tight_layout()
    fig.savefig(output_file)
    print(f"Saved {output_file}")


if __name__ == "__main__":
    main()
