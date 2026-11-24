
import os
import random

random.seed(7)
os.makedirs("sample_data", exist_ok=True)

n, depts = 1005, 42
dept_of = [min(int(random.paretovariate(1.2)) - 1, depts - 1) for _ in range(n)]
members = {}
for node, d in enumerate(dept_of):
    members.setdefault(d, []).append(node)

edges = []
for u in range(n):
    pool = members[dept_of[u]]
    for _ in range(random.randint(3, 40)):
        v = random.choice(pool) if random.random() < 0.7 else random.randrange(n)
        edges.append((u, v))          # may contain self-loops and duplicates on purpose

with open("sample_data/email-Eu-core.txt", "w") as f:
    for u, v in edges:
        f.write(f"{u} {v}\n")
with open("sample_data/email-Eu-core-department-labels.txt", "w") as f:
    for node, d in enumerate(dept_of):
        f.write(f"{node} {d}\n")
print("wrote sample_data/ (fake data, for testing only)")
