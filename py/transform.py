import csv
import os

script_dir = os.path.dirname(os.path.abspath(__file__))
INPUT_FILE = os.path.join(script_dir, 'IntGrid.csv')
OUTPUT_C = os.path.join(script_dir, 'level.c')
OUTPUT_H = os.path.join(script_dir, 'level.h')
SPIKE_ID = 4

def transform_spikes():
    if not os.path.exists(INPUT_FILE):
        print(f"Error: {INPUT_FILE} not found.")
        return

    with open(INPUT_FILE, mode='r') as f:
        reader = csv.reader(f)
        grid = [[int(cell) for cell in row if cell.strip()] for row in reader]

    rows = len(grid)
    cols = len(grid[0])
    new_grid = [row[:] for row in grid]
    
    stats = {10: 0, 11: 0, 12: 0, 13: 0}

    for y in range(rows):
        for x in range(cols):
            if grid[y][x] == SPIKE_ID:
                up    = grid[y-1][x] if y > 0 else -1
                down  = grid[y+1][x] if y < rows - 1 else -1
                left  = grid[y][x-1] if x > 0 else -1
                right = grid[y][x+1] if x < cols - 1 else -1

                new_val = SPIKE_ID
                if up != SPIKE_ID and down != SPIKE_ID:
                    if up == 0 or up == -1:
                        new_val = 10
                    elif down == 0 or down == -1:
                        new_val = 11
                elif left != SPIKE_ID and right != SPIKE_ID:
                    if left == 0 or left == -1:
                        new_val = 12
                    elif right == 0 or right == -1:
                        new_val = 13
                
                new_grid[y][x] = new_val
                if new_val in stats:
                    stats[new_val] += 1

    with open(OUTPUT_H, 'w') as f:
        f.write(f"#ifndef _LEVEL_H_\n#define _LEVEL_H_\n\n")
        f.write(f"#define MAP_W {cols}\n")
        f.write(f"#define MAP_H {rows}\n\n")
        f.write(f"extern const u16 map_collision_data[{cols * rows}];\n\n")
        f.write(f"#endif\n")

    with open(OUTPUT_C, 'w') as f:
        f.write(f'#include "level.h"\n\n')
        f.write(f"const u16 map_collision_data[{cols * rows}] = {{\n")
        
        for y in range(rows):
            line = ", ".join(map(str, new_grid[y]))
            f.write(f"    {line}")
            if y < rows - 1:
                f.write(",")
            f.write("\n")
            
        f.write("};\n\n")
        f.write("/*\n")
        f.write("  Spike Transformation Report:\n")
        f.write(f"  Tile 10 (Up):    {stats[10]}\n")
        f.write(f"  Tile 11 (Down):  {stats[11]}\n")
        f.write(f"  Tile 12 (Left):  {stats[12]}\n")
        f.write(f"  Tile 13 (Right): {stats[13]}\n")
        f.write("*/\n")

    print(f"Success: {OUTPUT_C} and {OUTPUT_H} created.")

if __name__ == "__main__":
    transform_spikes()