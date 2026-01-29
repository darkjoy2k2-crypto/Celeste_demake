import numpy as np
from PIL import Image
import sys
import os

def get_genesis_palette(image_path):
    """Extrahiert die Palette aus dem konvertierten Tileset."""
    img = Image.open(image_path)
    if img.mode != 'P':
        return None
    return img.getpalette()

def convert_tileset(input_path, output_path):
    """Konvertiert das Tileset und generiert die Master-Palette."""
    print(f"--- Tileset Konvertierung: {input_path} ---")
    img_orig = Image.open(input_path).convert("RGBA")
    
    # Transparenz-Maske
    alpha = np.array(img_orig)[:, :, 3]
    transparent_mask = alpha < 128

    img_rgb = img_orig.convert("RGB")
    steps = np.array([0, 36, 73, 109, 146, 182, 219, 255])
    data = np.array(img_rgb)
    data = steps[np.argmin(np.abs(data[:, :, :, np.newaxis] - steps), axis=3)]
    img_rounded = Image.fromarray(data.astype(np.uint8))

    # 15 Farben + 1 Slot für Transparenz (Index 0)
    img_quantized = img_rounded.quantize(colors=15, method=Image.Quantize.MAXCOVERAGE)
    raw_palette = img_quantized.getpalette()[:45]
    
    final_palette = [0, 0, 0] # Index 0 ist Schwarz/Transparent
    final_palette.extend(raw_palette)
    while len(final_palette) < 48: final_palette.append(0)

    palette_img = Image.new("P", (1, 1))
    palette_img.putpalette(final_palette)
    
    final_img = img_rounded.quantize(palette=palette_img, dither=Image.Dither.NONE)
    pixels = np.array(final_img)
    pixels[transparent_mask] = 0
    
    final_img = Image.fromarray(pixels.astype(np.uint8), mode="P")
    final_img.putpalette(final_palette)
    final_img.save(output_path)
    return final_palette

def convert_level_with_palette(input_path, output_path, master_palette):
    """Konvertiert ein Level-Bild unter Verwendung der Master-Palette."""
    print(f"Konvertiere Level: {input_path}")
    img_orig = Image.open(input_path).convert("RGBA")
    alpha = np.array(img_orig)[:, :, 3]
    transparent_mask = alpha < 128

    # Wir nutzen die Palette des Tilesets als Referenzbild
    ref_pal_img = Image.new("P", (1, 1))
    ref_pal_img.putpalette(master_palette)

    # Das Level-Bild wird auf die exakt gleichen Farben gemappt
    img_rgb = img_orig.convert("RGB")
    # Wichtig: Kein Dithering, damit die Tiles "clean" bleiben
    final_img = img_rgb.quantize(palette=ref_pal_img, dither=Image.Dither.NONE)
    
    pixels = np.array(final_img)
    pixels[transparent_mask] = 0
    
    final_img = Image.fromarray(pixels.astype(np.uint8), mode="P")
    final_img.putpalette(master_palette)
    final_img.save(output_path)

if __name__ == "__main__":
    res_dir = "res"
    tileset_source = os.path.join(res_dir, "tileset.png")
    tileset_output = os.path.join(res_dir, "g_tileset.png")

    if not os.path.exists(tileset_source):
        print(f"Fehler: Master-Tileset {tileset_source} nicht gefunden!")
        sys.exit(1)

    # 1. Tileset konvertieren und Palette holen
    master_palette = convert_tileset(tileset_source, tileset_output)

    # 2. Alle Level_XX finden und mit dieser Palette konvertieren
    i = 0
    while True:
        level_name = f"level_{i}.png"
        level_path = os.path.join(res_dir, level_name)
        
        if not os.path.exists(level_path):
            break # Stop wenn keine weiteren Level_xx gefunden werden
        
        output_level = os.path.join(res_dir, f"g_level_{i}.png")
        convert_level_with_palette(level_path, output_level, master_palette)
        i += 1

    print(f"\nFertig! {i} Level wurden basierend auf {tileset_output} konvertiert.")