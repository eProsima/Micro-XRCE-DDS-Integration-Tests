#!/usr/bin/python

import sys
import matplotlib.pyplot as plt

data = []
with open(sys.argv[1], 'r') as f:
    next(f)
    for line in f:
        data.append(list(map(lambda x: int(x), line.split())))

payload_sizes = map(lambda x: 2**x, range(4, 16))
payload_sizes.append(63000)

latency_avg = []
latency_std = []
throughput = []

for s in payload_sizes:
    filtered_data = list(filter(lambda x: x[0] == s, data))
    filtered_row = filter(lambda x: x[3] == min(map(lambda x: x[3], filtered_data)), filtered_data)[0]
    latency_avg.append(filtered_row[3] / 1e3)
    latency_std.append(filtered_row[4] / 1e3)
    throughput.append(max(map(lambda x: x[2], filtered_data)) / 1e6)

    latency = [x[3] / 1e3 for x in filtered_data]
    throught_pub = [x[1] / 1e6 for x in filtered_data]
    throught_sub = [x[2] / 1e6 for x in filtered_data]
    lost_percentage = list(map(lambda x, y: max(((x - y) / x) * 100, 0), map(lambda x: float(x), throught_pub), map(lambda x: float(x), throught_sub)))

    fig, ax = plt.subplots()
    ax.scatter(throught_pub, latency)
    ax.set_ylim(bottom=1)
    ax.set_ylabel("Latency ($\mu$s)")
    ax.set_xlabel("Throughput (Mbps)")
    plt.title("Latency vs through (%i B of payload)" %s)
    plt.show()

fig, ax = plt.subplots()
plt.errorbar(payload_sizes, latency_avg, yerr=latency_std, uplims=True, lolims=True)
ax.set_xlabel("Payload size (B)")
ax.set_ylabel("Latency ($\mu$s)")
ax.set_ylim(bottom=0)
ax.set_xticks([0, 15000, 30000, 45000, 60000, 75000])
ax.set_xscale("log")
plt.show()

fig, ax = plt.subplots()
plt.errorbar(payload_sizes, throughput)
ax.set_xlabel("Payload size (B)")
ax.set_ylabel("Throughput (Mbps)")
ax.set_ylim(bottom=0)
ax.set_xticks([0, 15000, 30000, 45000, 60000, 75000])
ax.set_xscale("log")
plt.show()