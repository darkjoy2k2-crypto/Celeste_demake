import numpy as np
from PIL import Image
import os

TILE_SIZE = 8

def get_tile_variants(tile_data):
    """Gibt alle 4 Spiegelungen auf Index-Ebene zurück."""
    return [
        ("n", tile_data),
        ("h", np.flip(tile_data, axis=1)),
        ("v", np.flip(tile_data, axis=0)),
        ("hv", np.flip(np.flip(tile_data, axis=0), axis=1))
    ]

def smart_fix():
    res_dir = "res"
    tileset_path = os.path.join(res_dir, "g_cavernas_edited.png")
    
    if not os.path.exists(tileset_path):
        print("Fehler: g_cavernas_edited.png nicht gefunden.")
        return

    # 1. Master-Tileset als Indiziertes Bild laden
    ts_img = Image.open(tileset_path).convert("P")
    ts_data = np.array(ts_img)
    ts_palette = ts_img.getpalette()
    ts_h, ts_w = ts_data.shape
    
    # Library aus Paletten-Indizes aufbauen
    tile_library = []
    for y in range(0, ts_h, TILE_SIZE):
        for x in range(0, ts_w, TILE_SIZE):
            tile_library.append(ts_data[y:y+TILE_SIZE, x:x+TILE_SIZE])
    
    print(f"Library: {len(tile_library)} Master-Tiles geladen.")

    # 2. Alle g_level_X.png verarbeiten
    i = 0
    while True:
        lv_name = f"g_level_{i}.png"
        lv_path = os.path.join(res_dir, lv_name)
        if not os.path.exists(lv_path): break
        
        print(f"Fixe Indizes in {lv_name}...")
        lv_img = Image.open(lv_path).convert("P")
        lv_data = np.array(lv_img)
        new_lv_data = lv_data.copy()
        
        lh, lw = lv_data.shape
        tiles_fixed = 0

        for y in range(0, lh, TILE_SIZE):
            for x in range(0, lw, TILE_SIZE):
                current_tile = lv_data[y:y+TILE_SIZE, x:x+TILE_SIZE]
                
                best_diff = float("inf")
                best_tile_data = None
                
                # Vergleich auf Index-Ebene (sehr schnell)
                for ref_tile in tile_library:
                    for mode, variant in get_tile_variants(ref_tile):
                        # Differenz der Indizes berechnen
                        diff = np.sum(current_tile != variant)
                        if diff < best_diff:
                            best_diff = diff
                            best_tile_data = variant
                        if diff == 0: break
                    if best_diff == 0: break

                # Wenn Indizes nicht 100% identisch waren, überschreiben
                if best_diff > 0:
                    new_lv_data[y:y+TILE_SIZE, x:x+TILE_SIZE] = best_tile_data
                    tiles_fixed += 1

        # Speichern mit der exakten Tileset-Palette
        final_lv_img = Image.fromarray(new_lv_data, mode="P")
        final_lv_img.putpalette(ts_palette)
        final_lv_img.save(lv_path)
        
        print(f"  -> {tiles_fixed} Tiles korrigiert.")
        i += 1

if __name__ == "__main__":
    smart_fix()