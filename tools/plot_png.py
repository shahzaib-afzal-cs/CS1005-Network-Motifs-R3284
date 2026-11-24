import csv
import re
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

path = sys.argv[1]
note = sys.argv[2] if len(sys.argv) > 2 else ""
R = re.search(r"R(\d+)", path).group(1)

classes, za, zb = [], [], []
with open(path) as f:
    for row in csv.DictReader(f):
        classes.append(int(row["class"]))
        za.append(float(row["Z_A"]))
        zb.append(float(row["Z_B"]))

w = 0.38
fig, ax = plt.subplots(figsize=(11, 5.5))
ax.bar([c - w / 2 for c in classes], za, w, label="Department A", color="#2f6db5")
ax.bar([c + w / 2 for c in classes], zb, w, label="Department B", color="#e07b24")
ax.axhline(2, color="red", linestyle="--", linewidth=1.3, label="Z = +2 / -2")
ax.axhline(-2, color="red", linestyle="--", linewidth=1.3)
ax.axhline(0, color="black", linewidth=0.8)
ax.set_xticks(classes)
ax.set_xlabel("Triad class")
ax.set_ylabel("Z-score")
ax.set_title(f"Triad Z-scores, R = {R} {note}".strip())
ax.legend()
fig.tight_layout()
out = path.replace(".csv", ".png")
fig.savefig(out, dpi=150)
print("saved", out)
