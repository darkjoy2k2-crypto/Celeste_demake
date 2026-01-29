IMAGE layer_bg "layer_bg.png" BEST ALL
IMAGE img_message_sign "message_sign.png" BEST ALL

TILESET level_0_tileset "level_0.png" BEST ALL
MAP level_0_mapdefinition "level_0.png" level_0_tileset  BEST 0

TILESET level_1_tileset "level_1.png" BEST ALL
MAP level_1_mapdefinition "level_1.png" level_1_tileset  BEST 0

TILESET level_2_tileset "level_2.png" BEST ALL
MAP level_2_mapdefinition "level_2.png" level_2_tileset  BEST 0


PALETTE pal_bg "layer_bg.png"
PALETTE pal_layer_1 "level_0.png" 
PALETTE pal_font "my_font.png"

SPRITE player_sprite "ball.png" 2 2 FAST 0
SPRITE stone_sprite "stone.png" 2 2 FAST 0
SPRITE stone2_sprite "stone2.png" 4 2 FAST 0

TILESET my_font_tiles "my_font.png" 0