#include <zephyr/kernel.h>
#include <drivers/behavior.h>
#include <stdio.h>
#include <string.h>

#include "layer_arc.h"
#include "../assets/custom_fonts.h"
#include <zmk/physical_layouts.h>
#include <zmk/keymap.h>
#include <zmk/matrix.h>

// LOCAL PATCH (zmk-config): connected-device label -----------------------------
// Hardcoded names for ZMK's five BLE profiles: slot N is whichever host is
// paired to profile N (managed with &bt BT_SEL/BT_CLR). Edit to taste.
static const char *const profile_names[] = {
    "SLOT0", "SLOT1", "SLOT2", "SLOT3", "SLOT4",
};

// Small label (same font as the USB/BLE indicator) directly above the layer
// name, showing the connected host: "USB" over USB, profile name over BLE.
static void draw_device_name(lv_obj_t *canvas, const struct status_state *state) {
    const char *name = "USB";

    if (state->selected_endpoint.transport == ZMK_TRANSPORT_BLE) {
        int idx = state->active_profile_index;
        if (idx < 0 || idx >= (int)(sizeof(profile_names) / sizeof(profile_names[0]))) {
            idx = 0;
        }
        name = profile_names[idx];
    }

    lv_draw_label_dsc_t name_dsc;
    init_label_dsc(&name_dsc, LVGL_FOREGROUND, &quinquefive_8, LV_TEXT_ALIGN_RIGHT);
    lv_canvas_draw_text(canvas, -23, 110, SCREEN_WIDTH, &name_dsc, name);
}
// END LOCAL PATCH --------------------------------------------------------------

void draw_layer_status(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_label_dsc_t label_dsc;
    // LOCAL PATCH (zmk-config): quinquefive_14 (locally generated size)
    // instead of quinquefive_18 so layer names up to ~6 chars fit on one
    // line (18 wrapped after ~5, 12 fit ~7).
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &quinquefive_14, LV_TEXT_ALIGN_RIGHT);

    char fallback_layer_name[16];

    const char *layer_name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(state->layer_index));

    if (layer_name == NULL || layer_name[0] == '\0') {
        sprintf(fallback_layer_name, "L#%" PRIu8, state->layer_index);

        layer_name = fallback_layer_name;
    }

    // LOCAL PATCH (zmk-config): y 115 -> 120 to keep the bottom edge of the
    // smaller font where the old one was; device-name label above it.
    lv_canvas_draw_text(canvas, -23, 120, SCREEN_WIDTH, &label_dsc, layer_name);
    draw_device_name(canvas, state);
}
