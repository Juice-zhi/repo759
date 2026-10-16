"""Plot the HW03 timing results.

Usage: python plot_hw03.py

Reads the timing files written by the Slurm scripts (one "x time_ms" pair per
line) and, for every file that exists, saves the plot as a PDF plus a 300 dpi
PNG copy for assignment3.docx:
    task1_timings.txt    -> task1.pdf     mmul time vs. threads (linear-linear)
    task2_timings.txt    -> task2.pdf     convolve time vs. threads (linear-linear)
    task3_ts_timings.txt -> task3_ts.pdf  msort time vs. threshold (linear time, log ts)
    task3_t_timings.txt  -> task3_t.pdf   msort time vs. threads (linear-linear)
It also prints each sweep as a table, with the speedup over one thread.
"""
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
matplotlib.rcParams["pdf.fonttype"] = 42  # embed TrueType so PDF text stays selectable
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, FuncFormatter, NullLocator

SERIES_COLOR = "#2a78d6"
IDEAL_COLOR = "#898781"
SURFACE = "#ffffff"
TEXT_PRIMARY = "#0b0b0b"
TEXT_SECONDARY = "#52514e"
GRID_COLOR = "#e1e0d9"
AXIS_COLOR = "#c3c2b7"


def read_timings(path):
    xs, times = [], []
    with open(path) as f:
        for line in f:
            fields = line.split()
            if len(fields) >= 2:
                xs.append(int(fields[0]))
                times.append(float(fields[1]))
    return xs, times


def fastest(times):
    return min(range(len(times)), key=times.__getitem__)


def new_axes(title, subtitle, xlabel):
    fig, ax = plt.subplots(figsize=(6.5, 4.2), facecolor=SURFACE)
    ax.set_facecolor(SURFACE)
    ax.set_title(title, color=TEXT_PRIMARY, loc="left", pad=20)
    ax.text(0, 1.02, subtitle, transform=ax.transAxes, ha="left", va="bottom",
            fontsize=9, color=TEXT_SECONDARY)
    ax.set_xlabel(xlabel, color=TEXT_SECONDARY)
    ax.set_ylabel("time (ms)", color=TEXT_SECONDARY)
    ax.grid(True, which="major", color=GRID_COLOR, linewidth=0.75)
    ax.set_axisbelow(True)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color(AXIS_COLOR)
    ax.tick_params(which="both", color=AXIS_COLOR, labelcolor=TEXT_SECONDARY)
    return fig, ax


def plot_measured(ax, xs, times):
    ax.plot(xs, times, color=SERIES_COLOR, linewidth=1.5, marker="o", markersize=6,
            markeredgecolor=SURFACE, markeredgewidth=1.5, label="measured",
            solid_joinstyle="round", solid_capstyle="round", zorder=3)
    # Ring around the fastest point; its value is given in the subtitle.
    best = fastest(times)
    ax.plot([xs[best]], [times[best]], linestyle="none", marker="o", markersize=13,
            markerfacecolor="none", markeredgecolor=SERIES_COLOR, markeredgewidth=1.25,
            zorder=4)


def save(fig, output):
    fig.tight_layout()
    png = Path(output).with_suffix(".png")
    fig.savefig(output)
    fig.savefig(png, dpi=300)
    plt.close(fig)
    print(f"Saved {output} and {png}\n")


def plot_thread_sweep(timings, output, title, details):
    ts, times = read_timings(timings)
    best = fastest(times)
    base = times[ts.index(1)] if 1 in ts else None

    print(f"{timings}:\n   t   time (ms)   speedup")
    for t, time in zip(ts, times):
        speedup = f"{base / time:7.2f}x" if base else ""
        print(f"{t:4d} {time:11.4g}   {speedup}")

    subtitle = f"{details}  ·  fastest: {times[best]:.3g} ms at t = {ts[best]}"
    if base:
        subtitle += f" ({base / times[best]:.1f}× faster than 1 thread)"
    fig, ax = new_axes(title, subtitle, "number of threads t")
    if base:
        ax.plot(ts, [base / t for t in ts], color=IDEAL_COLOR, linewidth=1.25,
                linestyle=(0, (4, 3)), label="ideal: T(1) / t", zorder=2)
    plot_measured(ax, ts, times)
    ax.xaxis.set_major_locator(FixedLocator(ts))
    ax.set_xlim(min(ts) - 0.5, max(ts) + 0.5)
    ax.set_ylim(bottom=0)
    if base:
        handles, labels = ax.get_legend_handles_labels()
        ax.legend(handles[::-1], labels[::-1], frameon=False, loc="upper right",
                  labelcolor=TEXT_SECONDARY)  # "measured" first
    save(fig, output)


def plot_threshold_sweep(timings, output, title, details):
    tss, times = read_timings(timings)
    best = fastest(times)

    print(f"{timings}:\n    ts   time (ms)")
    for ts, time in zip(tss, times):
        print(f"{ts:6d} {time:11.4g}")

    subtitle = f"{details}  ·  fastest: ts = {tss[best]} ({times[best]:.3g} ms)"
    fig, ax = new_axes(title, subtitle, "threshold ts (log scale)")
    plot_measured(ax, tss, times)
    ax.set_xscale("log", base=2)
    ax.xaxis.set_major_locator(FixedLocator(tss))
    ax.xaxis.set_major_formatter(FuncFormatter(lambda v, _: f"{v:g}"))
    ax.xaxis.set_minor_locator(NullLocator())
    ax.set_ylim(bottom=0)
    save(fig, output)


def main():
    found = False
    if Path("task1_timings.txt").exists():
        found = True
        plot_thread_sweep("task1_timings.txt", "task1.pdf",
                          "Problem 1: mmul time vs. number of threads", "n = 1024")
    if Path("task2_timings.txt").exists():
        found = True
        plot_thread_sweep("task2_timings.txt", "task2.pdf",
                          "Problem 2: convolve time vs. number of threads",
                          "n = 1024, 3 × 3 mask")
    best_ts = None
    if Path("task3_ts_timings.txt").exists():
        found = True
        tss, times = read_timings("task3_ts_timings.txt")
        best_ts = tss[fastest(times)]
        plot_threshold_sweep("task3_ts_timings.txt", "task3_ts.pdf",
                             "Problem 3: msort time vs. threshold",
                             "n = $10^6$, t = 8 threads")
    if Path("task3_t_timings.txt").exists():
        found = True
        details = "n = $10^6$" + (f", ts = {best_ts}" if best_ts else "")
        plot_thread_sweep("task3_t_timings.txt", "task3_t.pdf",
                          "Problem 3: msort time vs. number of threads", details)
    if not found:
        print("No *_timings.txt files found here; copy them from Euler first.")


if __name__ == "__main__":
    main()
