#include "city_overlay_risks.h"

#include "building/industry.h"
#include "game/state.h"
#include "graphics/image.h"
#include "map/building.h"
#include "map/property.h"
#include "map/random.h"
#include "map/terrain.h"

static int is_problem_cartpusher(int figure_id)
{
    if (figure_id) {
        figure *fig = figure_get(figure_id);
        return fig->actionState == FIGURE_ACTION_20_CARTPUSHER_INITIAL && fig->minMaxSeen;
    } else {
        return 0;
    }
}

void overlay_problems_prepare_building(building *b)
{
    if (b->houseSize) {
        return;
    }
    if (b->type == BUILDING_FOUNTAIN || b->type == BUILDING_BATHHOUSE) {
        if (!b->hasWaterAccess) {
            b->showOnProblemOverlay = 1;
        }
    } else if (b->type >= BUILDING_WHEAT_FARM && b->type <= BUILDING_CLAY_PIT) {
        if (is_problem_cartpusher(b->figureId)) {
            b->showOnProblemOverlay = 1;
        }
    } else if (building_is_workshop(b->type)) {
        if (is_problem_cartpusher(b->figureId)) {
            b->showOnProblemOverlay = 1;
        } else if (b->loadsStored <= 0) {
            b->showOnProblemOverlay = 1;
        }
    }
}

static int show_building_fire_crime(const building *b)
{
    return b->type == BUILDING_PREFECTURE || b->type == BUILDING_BURNING_RUIN;
}

static int show_building_damage(const building *b)
{
    return b->type == BUILDING_ENGINEERS_POST;
}

static int show_building_problems(const building *b)
{
    return b->showOnProblemOverlay;
}

static int show_building_native(const building *b)
{
    return b->type == BUILDING_NATIVE_HUT || b->type == BUILDING_NATIVE_MEETING || b->type == BUILDING_MISSION_POST;
}


static int show_figure_fire(const figure *f)
{
    return f->type == FIGURE_PREFECT;
}

static int show_figure_damage(const figure *f)
{
    return f->type == FIGURE_ENGINEER;
}

static int show_figure_crime(const figure *f)
{
    return f->type == FIGURE_PREFECT || f->type == FIGURE_PROTESTER ||
        f->type == FIGURE_CRIMINAL || f->type == FIGURE_RIOTER;
}

static int show_figure_problems(const figure *f)
{
    if (f->type == FIGURE_LABOR_SEEKER) {
        return building_get(f->buildingId)->showOnProblemOverlay;
    } else if (f->type == FIGURE_CART_PUSHER) {
        return f->actionState == FIGURE_ACTION_20_CARTPUSHER_INITIAL || f->minMaxSeen;
    } else {
        return 0;
    }
}

static int show_figure_native(const figure *f)
{
    return f->type == FIGURE_INDIGENOUS_NATIVE || f->type == FIGURE_MISSIONARY;
}


static int get_column_height_fire(const building *b)
{
    return b->fireRisk > 0 ? b->fireRisk / 10 : NO_COLUMN;
}

static int get_column_height_damage(const building *b)
{
    return b->damageRisk > 0 ? b->damageRisk / 10 : NO_COLUMN;
}

static int get_column_height_crime(const building *b)
{
    if (b->houseSize) {
        int happiness = b->sentiment.houseHappiness;
        if (happiness <= 0) {
            return 10;
        } else if (happiness <= 10) {
            return 8;
        } else if (happiness <= 20) {
            return 6;
        } else if (happiness <= 30) {
            return 4;
        } else if (happiness <= 40) {
            return 2;
        } else if (happiness < 50) {
            return 1;
        }
    }
    return NO_COLUMN;
}

static int get_column_height_none(const building *b)
{
    return NO_COLUMN;
}


static int get_tooltip_fire(tooltip_context *c, const building *b)
{
    if (b->fireRisk <= 0) {
        return 46;
    } else if (b->fireRisk <= 20) {
        return 47;
    } else if (b->fireRisk <= 40) {
        return 48;
    } else if (b->fireRisk <= 60) {
        return 49;
    } else if (b->fireRisk <= 80) {
        return 50;
    } else {
        return 51;
    }
}

static int get_tooltip_damage(tooltip_context *c, const building *b)
{
    if (b->damageRisk <= 0) {
        return 52;
    } else if (b->damageRisk <= 40) {
        return 53;
    } else if (b->damageRisk <= 80) {
        return 54;
    } else if (b->damageRisk <= 120) {
        return 55;
    } else if (b->damageRisk <= 160) {
        return 56;
    } else {
        return 57;
    }
}

static int get_tooltip_crime(tooltip_context *c, const building *b)
{
    if (b->sentiment.houseHappiness <= 0) {
        return 63;
    } else if (b->sentiment.houseHappiness <= 10) {
        return 62;
    } else if (b->sentiment.houseHappiness <= 20) {
        return 61;
    } else if (b->sentiment.houseHappiness <= 30) {
        return 60;
    } else if (b->sentiment.houseHappiness < 50) {
        return 59;
    } else {
        return 58;
    }
}

const city_overlay *city_overlay_for_fire()
{
    static city_overlay overlay = {
        OVERLAY_FIRE,
        COLUMN_TYPE_RISK,
        show_building_fire_crime,
        show_figure_fire,
        get_column_height_fire,
        0,
        get_tooltip_fire,
        0,
        0
    };
    return &overlay;
}

const city_overlay *city_overlay_for_damage()
{
    static city_overlay overlay = {
        OVERLAY_DAMAGE,
        COLUMN_TYPE_RISK,
        show_building_damage,
        show_figure_damage,
        get_column_height_damage,
        0,
        get_tooltip_damage,
        0,
        0
    };
    return &overlay;
}

const city_overlay *city_overlay_for_crime()
{
    static city_overlay overlay = {
        OVERLAY_CRIME,
        COLUMN_TYPE_RISK,
        show_building_fire_crime,
        show_figure_crime,
        get_column_height_crime,
        0,
        get_tooltip_crime,
        0,
        0
    };
    return &overlay;
}

const city_overlay *city_overlay_for_problems()
{
    static city_overlay overlay = {
        OVERLAY_PROBLEMS,
        COLUMN_TYPE_RISK,
        show_building_problems,
        show_figure_problems,
        get_column_height_none,
        0,
        0,
        0,
        0
    };
    return &overlay;
}

static int terrain_on_native_overlay()
{
    return
        TERRAIN_TREE | TERRAIN_ROCK | TERRAIN_WATER | TERRAIN_SCRUB |
        TERRAIN_GARDEN | TERRAIN_ELEVATION | TERRAIN_ACCESS_RAMP | TERRAIN_RUBBLE;
}

static void draw_footprint_native(int x, int y, int grid_offset)
{
    if (!map_property_is_draw_tile(grid_offset)) {
        return;
    }
    if (map_terrain_is(grid_offset, terrain_on_native_overlay())) {
        if (map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
            city_with_overlay_draw_building_footprint(x, y, grid_offset, 0);
        } else {
            draw_foot_with_size(grid_offset, x, y);
        }
    } else if (map_terrain_is(grid_offset, TERRAIN_AQUEDUCT | TERRAIN_WALL)) {
        // display grass
        int image_id = image_group(GROUP_TERRAIN_GRASS_1) + (map_random_get(grid_offset) & 7);
        image_draw_isometric_footprint(image_id, x, y, 0);
    } else if (map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
        city_with_overlay_draw_building_footprint(x, y, grid_offset, 0);
    } else {
        if (map_property_is_native_land(grid_offset)) {
            image_draw_isometric_footprint(image_group(GROUP_TERRAIN_DESIRABILITY) + 1, x, y, 0);
        } else {
            draw_foot_with_size(grid_offset, x, y);
        }
    }
}

static void draw_top_native(int x, int y, int grid_offset)
{
    if (!map_property_is_draw_tile(grid_offset)) {
        return;
    }
    if (map_terrain_is(grid_offset, terrain_on_native_overlay())) {
        if (!map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
            draw_top_with_size(grid_offset, x, y);
        }
    } else if (map_building_at(grid_offset)) {
        city_with_overlay_draw_building_top(x, y, grid_offset);
    }
}

const city_overlay *city_overlay_for_native()
{
    static city_overlay overlay = {
        OVERLAY_NATIVE,
        COLUMN_TYPE_RISK,
        show_building_native,
        show_figure_native,
        get_column_height_none,
        0,
        0,
        draw_footprint_native,
        draw_top_native
    };
    return &overlay;
}