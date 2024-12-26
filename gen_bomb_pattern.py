#!/usr/bin/env python3

def parse_patterns(file):
    patterns = {}
    
    cur_pat_name = None
    cur_pat = []
    for line in file.readlines():
        if len(line) < 3:
            continue
        
        if line[0] == "-":
            if cur_pat_name is not None:
                patterns[cur_pat_name] = cur_pat
                cur_pat = []
            cur_pat_name = line[2:].strip()
            continue
            
        # parse pat line
        cur_pat.append(line.strip().replace(' ', ''))


    if cur_pat_name is not None:
        patterns[cur_pat_name] = cur_pat
        cur_pat = []

    for p, lines in patterns.items():
        tiles = [(x, y) for x in range(len(lines[0])) for y in range(len(lines)) if lines[y][x] == '*']
        if len(tiles) > 0:
            patterns[p] = tiles
        else:
            patterns[p] = None

    return patterns


def calc_potentials(patterns):
    def calc_potential(tiles):
        pot = 0
        for i, tile_i in enumerate(tiles):
            for j, tile_j in enumerate(tiles):
                if i > j:
                    pot += 1.0 / ((tile_i[0] - tile_j[0])**2 + (tile_i[1] - tile_j[1])**2)
        return pot
            
    for p, tiles in patterns.items():
        if patterns[p] is not None:
            patterns[p] = calc_potential(tiles)


def dump_cpp(patterns):
    print("""
const std::pair<const char*, lif::PatternSignature> bombPatterns[] = {""")
    print(',\n'.join(f'\t{{ "{p}", {round(v, 8)}f }}' for p, v in patterns.items() if v is not None))
    print("};")
        
        
    
if __name__ == "__main__":
    with open("bomb_patterns.txt") as file:
        patterns = parse_patterns(file)
        calc_potentials(patterns)
        dump_cpp(patterns)        
