import numpy as np
from PIL import Image
import sys
import os

TILE_SIZE = 8

def get_tile_variants(tile_data):
    """Generiert alle 4 gespiegelten Varianten eines Tiles."""
    variants = {
        "normal": tile_data,
        "h-flip": np.flip(tile_data, axis=1),
        "v-flip": np.flip(tile_data, axis=0),
        "hv-flip": np.flip(np.flip(tile_data, axis=0), axis=1)
    }
    return variants

def image_to_tiles(img):
    """Zerlegt ein Bild in 8x8 Tiles."""
    img = img.convert("RGB")
    data = np.array(img)
    h, w, _ = data.shape
    tiles = []
    for y in range(0, h, TILE_SIZE):
        for x in range(0, w, TILE_SIZE):
            tile = data[y:y+TILE_SIZE, x:x+TILE_SIZE]
            tiles.append({'pos': (x, y), 'data': tile})
    return tiles

def run_diagnose(tileset_path, level_path):
    print(f"\n--- Diagnose: {os.path.basename(level_path)} ---")
    
    ts_img = Image.open(tileset_path)
    lv_img = Image.open(level_path)
    
    # Master-Tileset Library aufbauen
    ts_tiles = image_to_tiles(ts_img)
    tile_library = []
    for t in ts_tiles:
        # Wir speichern die Hashes der 4 Varianten für schnellen Vergleich
        variants = get_tile_variants(t['data'])
        tile_library.append({
            'variants': {k: hash(v.tobytes()) for k, v in variants.items()},
            'pos': t['pos']
        })

    # Level Tiles prüfen
    lv_tiles = image_to_tiles(lv_img)
    missing_count = 0
    
    for lt in lv_tiles:
        lt_hash = hash(lt['data'].tobytes())
        found = False
        
        for idx, ts_entry in enumerate(tile_library):
            for mode, h in ts_entry['variants'].items():
                if lt_hash == h:
                    found = True
                    break
            if found: break
        
        if not found:
            missing_count += 1
            if missing_count <= 5: # Nur die ersten 5 Fehler zeigen
                print(f"  [!] Unbekanntes Tile bei Pixel {lt['pos']}")

    if missing_count == 0:
        print(f"  [OK] Alle Tiles im Tileset gefunden (inkl. Mirroring).")
    else:
        print(f"  [FEHLER] {missing_count} Tiles im Level sind NICHT im Tileset vorhanden!")

if __name__ == "__main__":
    # Pfade festlegen wie besprochen
    base_path = "res"
    tileset = os.path.join(base_path, "g_cavernas_edited.png")
    levels = ["g_level_0.png", "g_level_1.png", "g_level_2.png", "g_level_3.png"]

    if not os.path.exists(tileset):
        print(f"Tileset nicht gefunden: {tileset}")
        sys.exit(1)

    for lv in levels:
        lv_path = os.path.join(base_path, lv)
        if os.path.exists(lv_path):
            run_diagnose(tileset, lv_path)
        else:
            print(f"Überspringe {lv} (Datei existiert nicht).")