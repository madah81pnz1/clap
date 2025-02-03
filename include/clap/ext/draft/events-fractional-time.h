#pragma once

#include "../events.h"

/// @page Events Fractional Time
///
/// Provides support for fractional sample offset in 1/4294967296ths of sample. 0x80000000 == 0.5
/// samples
///
/// The events must be sorted according to their time and fractional_time values.
///
static CLAP_CONSTEXPR const char CLAP_EXT_EVENTS_FRACTIONAL_TIME[] =
   "clap.events-fractional-time/1";

#ifdef __cplusplus
extern "C"_t {
#endif

// fractional sample offset in 1/4294967296ths of sample. 0x80000000 == 0.5
typedef uint32_t clap_fractional_time;

enum clap_note_port_fractional_time_flags {
   // This note port supports fractional time events
   CLAP_FRACTIONAL_TIME_EVENTS_SUPPORTED = 1 << 0,
};

typedef struct clap_note_port_fractional_time_info {
   // Must be the same id as an existing note port.
   clap_id  id;
   uint32_t flags; // bitfield, see clap_note_port_fractional_time_flags
} clap_note_port_fractional_time_info_t;

// The note ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_note_ports_fractional_time {
   // Number of ports, for either input or output.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin, bool is_input);

   // Get info about a note port.
   // Returns true on success and stores the result into info.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t                   *plugin,
                       uint32_t                               index,
                       bool                                   is_input,
                       clap_note_port_fractional_time_info_t *info);
} clap_plugin_note_ports_fractional_time_t;

// Given that this extension is still draft, it'll use the event-registry and its own event
// namespace until we stabilize it.
//
// #include <clap/ext/event-registry.h>
//
// uint16_t CLAP_EXT_FRACTIONAL_TIME_EVENT_SPACE_ID = UINT16_MAX;
// if (host_event_registry->query(host, CLAP_EXT_EVENTS_FRACTIONAL_TIME,
// &CLAP_EXT_FRACTIONAL_TIME_EVENT_SPACE_ID)) {
//   /* we can use fractional time events */
// }

/// These new fractional time event structures are done like this to be compatible with the existing
/// event structures.

typedef struct clap_event_fractional_time_header_t {
   clap_event_header_t  header;
   clap_fractional_time fractional_time; // fractional sample offset
} clap_event_fractional_time_header;

typedef struct clap_event_fractional_time_note_t {
   clap_event_note_t    note;
   clap_fractional_time fractional_time; // fractional sample offset
} clap_event_fractional_time_note;

typedef struct clap_event_fractional_time_note_expression_t {
   clap_event_note_expression_t note_expression;
   clap_fractional_time         fractional_time; // fractional sample offset
} clap_event_fractional_time_note_expression;

typedef struct clap_event_fractional_time_param_value_t {
   clap_event_param_value_t param_value;
   clap_fractional_time     fractional_time; // fractional sample offset
} clap_event_fractional_time_param_value;

typedef struct clap_event_fractional_time_param_mod_t {
   clap_event_param_mod_t param_mod;
   clap_fractional_time   fractional_time; // fractional sample offset
} clap_event_fractional_time_param_mod;

typedef struct clap_event_fractional_time_param_gesture_t {
   clap_event_param_gesture_t param_gesture;
   clap_fractional_time       fractional_time; // fractional sample offset
} clap_event_fractional_time_param_gesture;

typedef struct clap_event_fractional_time_transport_t {
   clap_event_transport_t transport;
   clap_fractional_time   fractional_time; // fractional sample offset
} clap_event_fractional_time_transport;

typedef struct clap_event_fractional_time_midi_t {
   clap_event_midi_t    midi;
   clap_fractional_time fractional_time; // fractional sample offset
} clap_event_fractional_time_midi;

typedef struct clap_event_fractional_time_midi_sysex_t {
   clap_event_midi_sysex_t midi_sysex;
   clap_fractional_time    fractional_time; // fractional sample offset
} clap_event_fractional_time_midi_sysex;

typedef struct clap_event_fractional_time_midi2_t {
   clap_event_midi2_t   midi2;
   clap_fractional_time fractional_time; // fractional sample offset
} clap_event_fractional_time_midi2;

#ifdef __cplusplus
}
#endif
