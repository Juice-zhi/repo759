"""Plot the Problem 4 timings into task4.pdf (one panel per OpenMP schedule).

Usage: python plot_task4.py

Reads task4_timings.txt written by run_task4.sh (one "schedule t time_ms" line
per run, several runs per configuration) and plots the fastest run of each
configuration. Saves task4.pdf plus a 300 dpi PNG copy for assignment4.docx and
prints every run with the speedup of the fastest one over one thread.
"""
from collections import defaultdict

import matplotlib

matplotlib.use("Agg")
matplotlib.rcParams["pdf.fonttype"] = 42  # embed TrueType so PDF text stays selectable
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator

SCHEDULES = ["static", "dynamic", "guided"]
SERIES_COLOR = "#2a78d6"
IDEAL_COLOR = "#898781"
SURFACE = "#ffffff"
TEXT_PRIMARY = "#0b0b0b"
TEXT_SECONDARY = "#52514e"
GRID_COLOR = "#e1e0d9"
AXIS_COLOR = "#c3c2b7"


def read_timings(path):
    """Returns {schedule: {t: [time_ms of every run]}}."""
    runs = defaultdict(lambda: defaultdict(list))
    with open(path) as f:
        for line in f:
            fields = line.split()
            if len(fields) >= 3:
                runs[fields[0]][int(fields[1])].append(float(fields[2]))
    return runs


def main():
    runs = read_timings("task4_timings.txt")
    schedules = [s for s in SCHEDULES if s in runs]
    repeats = max(len(times) for s in schedules for times in runs[s].values())

    fig, axes = plt.subplots(len(schedules), 1, figsize=(6.5, 2.35 * len(schedules) + 0.6),
                             sharex=True, sharey=True, facecolor=SURFACE, squeeze=False)
    for ax, schedule in zip(axes[:, 0], schedules):
        ts = sorted(runs[schedule])
        times = [min(runs[schedule][t]) for t in ts]
        best = min(range(len(times)), key=times.__getitem__)
        base = times[ts.index(1)] if 1 in ts else None

        print(f"{schedule}:\n   t    fastest (ms)   speedup   all runs (ms)")
        for t, time in zip(ts, times):
            speedup = f"{base / time:6.2f}x" if base else ""
            every = ", ".join(f"{run:.6g}" for run in runs[schedule][t])
            print(f"{t:4d} {time:14.5g}   {speedup:>7}   {every}")
        print()

        title = f"{schedule}  ·  fastest: {times[best]:.4g} ms at t = {ts[best]}"
        if base:
            title += f" ({base / times[best]:.1f}× faster than 1 thread)"
            ax.plot(ts, [base / t for t in ts], color=IDEAL_COLOR, linewidth=1.25,
                    linestyle=(0, (4, 3)), label="ideal: T(1) / t", zorder=2)
        ax.plot(ts, times, color=SERIES_COLOR, linewidth=1.5, marker="o", markersize=6,
                markeredgecolor=SURFACE, markeredgewidth=1.5, label="measured",
                solid_joinstyle="round", solid_capstyle="round", zorder=3)
        ax.plot([ts[best]], [times[best]], linestyle="none", marker="o", markersize=13,
                markerfacecolor="none", markeredgecolor=SERIES_COLOR, markeredgewidth=1.25,
                zorder=4)

        ax.set_facecolor(SURFACE)
        ax.set_title(title, color=TEXT_PRIMARY, loc="left", fontsize=10)
        ax.grid(True, color=GRID_COLOR, linewidth=0.75)
        ax.set_axisbelow(True)
        for side in ("top", "right"):
            ax.spines[side].set_visible(False)
        for side in ("left", "bottom"):
            ax.spines[side].set_color(AXIS_COLOR)
        ax.tick_params(color=AXIS_COLOR, labelcolor=TEXT_SECONDARY)
        ax.xaxis.set_major_locator(FixedLocator(ts))
        ax.set_xlim(min(ts) - 0.5, max(ts) + 0.5)
        ax.set_ylim(bottom=0)

    handles, labels = axes[0, 0].get_legend_handles_labels()
    axes[0, 0].legend(handles[::-1], labels[::-1], frameon=False, loc="best",
                      labelcolor=TEXT_SECONDARY)  # "measured" first
    axes[-1, 0].set_xlabel("number of threads t", color=TEXT_SECONDARY)
    fig.supylabel("time (ms)", color=TEXT_SECONDARY, fontsize=10)
    subtitle = "N = 200 particles, simulation end time 100"
    if repeats > 1:
        subtitle += f"; each point is the fastest of {repeats} runs"
    fig.suptitle("Problem 4: N-body simulation time vs. number of threads\n" + subtitle,
                 color=TEXT_PRIMARY, x=0.02, ha="left", fontsize=11)
    fig.tight_layout()
    fig.savefig("task4.pdf")
    fig.savefig("task4.png", dpi=300)
    print("Saved task4.pdf and task4.png")


if __name__ == "__main__":
    main()
