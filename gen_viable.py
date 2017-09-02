#!/usr/bin/python3

# Utility to generate valid positions for GodEyeBoss

import sys

if len(sys.argv) < 2:
	print("Missing needed argument (tilemap)")
	sys.exit(1)

tilemap = sys.argv[1]
width = 15
i = 0
prev_line = 1
for ch in tilemap:
	if ch not in {'1', '2', '3', '+'}:
		line = i // width + 1
		print('{}{{{},{}}}'.format(' ' if line == prev_line else '\n', i % width + 1, line), end=',')
		prev_line = line
	i += 1
print('')
