import csv
import sys
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, FuncFormatter, NullFormatter

CSV_PATH = sys.argv[1] if len(sys.argv) > 1 else "sweep.csv"

# Per-core caches of an i5-12450HX P-core (L1d/L2 per core, L3 shared), in KiB
CACHES = [("L1d", 48), ("L2", 1280), ("L3", 12288)]

ws, scalar, avx2, speedup = [], [], [], []
with open(CSV_PATH) as f:
    for row in csv.DictReader(f):
        ws.append(float(row["working_set_kib"]))
        scalar.append(float(row["scalar_gbps"]))
        avx2.append(float(row["avx2_gbps"]))
        speedup.append(float(row["speedup"]))


def fmt_size(kib, _pos=None):
    return f"{kib / 1024:g} MiB" if kib >= 1024 else f"{kib:g} KiB"


fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(9, 8), sharex=True)

ax1.plot(ws, scalar, "o-", label="scalar")
ax1.plot(ws, avx2, "o-", label="AVX2")
ax1.set_ylabel("throughput (GB/s of memory traffic)")
ax1.set_title("Float multiply: scalar vs AVX2 across the cache hierarchy")
ax1.legend(loc="upper right")
ax1.grid(True, alpha=0.3)

ax2.plot(ws, speedup, "o-", color="green")
ax2.axhline(1, color="gray", linestyle="--")
ax2.set_ylabel("AVX2 speedup (x)")
ax2.set_xlabel("working set (all three arrays)")
ax2.set_ylim(bottom=0)
ax2.grid(True, alpha=0.3)

# Cache boundaries; labels use axes coordinates so they stay put whatever the data range
for name, kib in CACHES:
    for ax in (ax1, ax2):
        ax.axvline(kib, color="red", linestyle=":", alpha=0.6)
    ax2.text(
        kib,
        0.97,
        name,
        transform=ax2.get_xaxis_transform(),
        color="red",
        ha="right",
        va="top",
    )

# Log x-axis with readable size labels instead of 10^1, 10^2, ...
ax1.set_xscale("log")
ticks = [16, 64, 256, 1024, 4096, 16384, 65536, 262144]
ax2.xaxis.set_major_locator(FixedLocator(ticks))
ax2.xaxis.set_major_formatter(FuncFormatter(fmt_size))
ax2.xaxis.set_minor_formatter(NullFormatter())
ax2.set_xlim(min(ws) * 0.8, max(ws) * 1.25)

plt.tight_layout()
plt.savefig("sweep.png", dpi=150)
plt.show()
