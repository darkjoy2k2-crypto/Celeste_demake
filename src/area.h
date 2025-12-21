#ifndef _AREA_H_
#define _AREA_H_

#include <genesis.h>

#define CAM_MODE_FOLLOW 0
#define CAM_MODE_FIXED  1

typedef struct {
    Vect2D_s16 min;       
    Vect2D_s16 max;       
    Vect2D_s16 spawn;     
    Vect2D_s16 cam;      
    Vect2D_s16 cam_min;    
    Vect2D_s16 cam_max;      
    u16 cam_mode;
    u16 type;
} Area;

void load_areas(const Area* data, u16 count);
int get_current_area_id(s16 px, s16 py);
const Area* get_area(u16 id); // Geändert von int auf u16

#endif