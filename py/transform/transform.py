import csv
import os
import re

script_dir = os.path.dirname(os.path.abspath(__file__))
SPIKE_ID = 4

def sanitize_name(name):
    # Removes extension and replaces non-alphanumeric chars with underscores
    base = os.path.splitext(name)[0]
    return re.sub(r'[^a-zA-Z0-9_]', '_', base)

def process_file(csv_path):
    filename = os.path.basename(csv_path)
    var_name = sanitize_name(filename)
    output_c = os.path.join(script_dir, f"{var_name}.c")
    output_h = os.path.join(script_dir, f"{var_name}.h")

    with open(csv_path, mode='r') as f:
        reader = csv.reader(f)
        # Filters out empty rows/cells
        grid = [[int(cell) for cell in row if cell.strip()] for row in reader if row]

    if not grid:
        return

    rows = len(grid)
    cols = len(grid[0])
    new_grid = [row[:] for row in grid]
    stats = {10: 0, 11: 0, 12: 0, 13: 0}

    # Spike Transformation Logic
    for y in range(rows):
        for x in range(cols):
            if grid[y][x] == SPIKE_ID:
                up    = grid[y-1][x] if y > 0 else -1
                down  = grid[y+1][x] if y < rows - 1 else -1
                left  = grid[y][x-1] if x > 0 else -1
                right = grid[y][x+1] if x < cols - 1 else -1

                new_val = SPIKE_ID
                # Logic: Check surrounding empty space (0) or map bounds (-1)
                if up != SPIKE_ID and down != SPIKE_ID:
                    if up in (0, -1): new_val = 10
                    elif down in (0, -1): new_val = 11
                elif left != SPIKE_ID and right != SPIKE_ID:
                    if left in (0, -1): new_val = 12
                    elif right in (0, -1): new_val = 13
                
                new_grid[y][x] = new_val
                if new_val in stats:
                    stats[new_val] += 1

    # Write Header File
    with open(output_h, 'w') as f:
        guard = f"_{var_name.upper()}_H_"
        f.write(f"#ifndef {guard}\n#define {guard}\n\n")
        f.write(f"#include <genesis.h>\n\n")
        f.write(f"#define {var_name.upper()}_W {cols}\n")
        f.write(f"#define {var_name.upper()}_H {rows}\n\n")
        f.write(f"extern const u16 {var_name}_data[{cols * rows}];\n\n")
        f.write(f"#endif\n")

    # Write C File
    with open(output_c, 'w') as f:
        f.write(f'#include "{var_name}.h"\n\n')
        f.write(f"const u16 {var_name}_data[{cols * rows}] = {{\n")
        for y in range(rows):
            line = ", ".join(map(str, new_grid[y]))
            f.write(f"    {line}{',' if y < rows - 1 else ''}\n")
        f.write("};\n")

    print(f"Generated: {var_name}.c and .h ({stats})")

def transform_all_csv():
    files = [f for f in os.listdir(script_dir) if f.lower().endswith('.csv')]
    if not files:
        print("No CSV files found in directory.")
        return

    for csv_file in files:
        process_file(os.path.join(script_dir, csv_file))

if __name__ == "__main__":
    transform_all_csv()