#pragma once

#include "../audio-ports.h"

/// @page Audio Ports Data Types
///
/// This extension provides a way for the plugin to describe extended configuration for audio ports.
/// This includes support for fixed width floating-point types as defined in C23/C++23.
///
/// The idea is that the host would first query the plugin for normal audio-ports, and then query
/// all the extended audio-ports. All extended audio-ports must have a corresponding normal
/// audio-port with the same id, but not all normal audio-ports needs to have an extended
/// audio-port.
///
/// The plugin is only allowed to change its ports configuration while it is deactivated.
///
/// TODO:
///     Better name for this extension?
///     audio-ports-extended is too similar to extensible-audio-ports.
///     Something with fixed-width-floating-point-types is not so future-proof, since
///     this extension could work also for integer types or GMP/MPFR types.
///     audio-ports-v2 might suggest it's a new way of handling audio-ports,
///     incompatible with the old audio-ports.
///     audio-ports-additional-data-types? Or just audio-ports-data-types?
///
///     Should the audio port defined here be the same as the normal audio ports, or should they be
///     in a separate namespace? Since we already have other extensions: audio-port-config,
///     configurable-audio-ports, extensible-audio-ports; it would be good to avoid having to
///     duplicate and having two separate extensions for those. It might be too confusing to have
///     two different sets of audio-ports, since the only difference here is to be able to use other
///     audio sample types for the data stream. clap_plugin_audio_ports_config_info_t returns a
///     clap_audio_port_info_t, so there must be an new extension API for
///     clap_audio_port_data_types_info_t. Include the relevant parts in this
///     extension, or create another extension, e.g. audio-ports-config-extended.h?
///
///     Currently the _extended structs includes their original counterpart at the beginning. This
///     is to avoid duplicating all the info, and to allow the plugin to reuse the _ex struct in the
///     original audio-ports API. But, this also means the plugin and host needs to remember to
///     ignore some members and values. Might be better then to copy-paste the original struct and
///     change the members, even if it becomes harder to maintain if the original struct is updated?
///
///     The enum clap_audio_ports_data_types and struct clap_audio_port_data_types_info can be
///     removed, and instead new enum values could be added to the enum in audio-ports.h, e.g.
///     CLAP_AUDIO_PORT_SUPPORTS_128BITS. But that might also make it confusing, since the enum
///     value belongs to another extension. Same with clap_host_audio_ports_t, the flag
///     CLAP_AUDIO_PORTS_RESCAN_FLAGS could be reused in case the type changes. Then
///     clap_host_audio_ports_data_types_t won't be needed and can be removed.
///
///     Should it be allowed to mix process()/process-ex() calls? The plugin should be prepared to
///     get either a process or process-ex call, but there should be no need to call
///     stop_processing()/start_processing() in between. Or do we need a new
///     start_processing_extended()/stop_processing_extended()? In the normal process() call, it
///     would be allowed to switch from data32 to data64 in the next call, as long as the plugin
///     supports CLAP_AUDIO_PORT_SUPPORTS_64BITS. By the same reasoning, it should be allowed to
///     switch to process-ex in the next call. But if CLAP_AUDIO_PORT_REQUIRES_COMMON_SAMPLE_SIZE is
///     set, does that mean it is still ok to switch as long as all ports switch at the same time?
///     Not sure how to interpret this.
///
///     Is there a point of avoiding the redundancy between data32/float and float32_t, and
///     data64/double and float64_t? That is, if the plugin has CLAP_AUDIO_PORT_SUPPORTS_64BITS set
///     in the normal audio-port, should you expect it to also have
///     CLAP_AUDIO_PORTS_DATA_TYPES_FLOAT64? Or should we remove both
///     CLAP_AUDIO_PORTS_DATA_TYPES_FLOAT32/FLOAT64, but then the logic for the preferred type
///     becomes a bit convoluted, e.g.: real_preferred_type = (preferred_type
///     != 0) ? preferred_type : ((flags & CLAP_AUDIO_PORT_PREFERS_64BITS) ? PREFERS_DATA64 :
///     PREFERS_DATA32); An example of this would be if the plugin supports float16, but prefers to
///     use data64/double.
///
///     Maybe the preferred_type is not so useful?
///     An example is if the plugin supports Float128 but prefers Float64, since it uses
///     software emulation for Float128, making it too slow for realtime use. But there is no
///     way to inform the host of this. Maybe for another extension, to query the data type and
///     supported render mode.
///
static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_DATA_TYPES[] =
   "clap.audio-ports-data-types/1";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_audio_ports_data_types {
   // std::bfloat16_t
   CLAP_AUDIO_PORTS_DATA_TYPES_BFLOAT16 = 1 << 0,

   // _Float16/std::float16_t
   CLAP_AUDIO_PORTS_DATA_TYPES_FLOAT16 = 1 << 1,

   // _Float32/std::float32_t
   CLAP_AUDIO_PORTS_DATA_TYPES_FLOAT32 = 1 << 2,

   // _Float64/std::float64_t
   CLAP_AUDIO_PORTS_DATA_TYPES_FLOAT64 = 1 << 3,

   // _Float128/std::float128_t
   CLAP_AUDIO_PORTS_DATA_TYPES_FLOAT128 = 1 << 4,
};

typedef struct clap_audio_port_data_types_info {
   // See clap_audio_port_info in audio-ports.h
   // These two flag values in audio_port_info.flags must be ignored:
   // CLAP_AUDIO_PORT_SUPPORTS_64BITS
   // CLAP_AUDIO_PORT_PREFERS_64BITS
   //
   // TODO: audio_port_info here might not be needed, since the host would
   // need to query all the normal audio-ports first, and then query to see
   // if there are any extended audio-ports info.
   clap_audio_port_info_t audio_port_info;

   uint32_t supported_types; // bitfield, see clap_audio_ports_data_types
   uint32_t preferred_type;  // one value of clap_audio_ports_data_types
} clap_audio_port_data_types_info_t;

typedef struct clap_audio_buffer_data_type {
   // See clap_audio_buffer in audio-buffer.h
   // audio_buffer.data32 and audio_buffer.data64 pointers must be ignored
   //
   // TODO: audio_buffer (or its contents) must be included here, since
   // this is the argument sent in the process() call.
   // An alternative is to have two pointers in the process() call,
   // one for clap_audio_buffer_t and another for clap_audio_buffer_data_type_t.
   clap_audio_buffer_t audio_buffer;

   void   **data;      // See data_type for the type used
   uint32_t data_type; // one value of clap_audio_ports_data_types
} clap_audio_buffer_data_type_t;

typedef struct clap_process_data_type {
   // See clap_process in process.h
   // process.audio_inputs, process.audio_outputs, process.audio_inputs_count and
   // process.audio_outputs_count must be ignored.
   //
   // TODO: process (or its contents) must be included here, same comment as above
   // for clap_audio_buffer_data_type.
   clap_process_t process;

   // Audio buffers, they must have the same count as specified
   // by clap_plugin_audio_ports_data_types->count().
   // The index maps to clap_plugin_audio_ports_data_types->get().
   // Input buffer and its contents are read-only.
   const clap_audio_buffer_data_type_t *audio_inputs;
   clap_audio_buffer_data_type_t       *audio_outputs;

} clap_process_data_type_t;

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports_data_types {
   // Number of ports, for either input or output
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin, bool is_input);

   // Get extended info about an audio port.
   // Returns true on success and stores the result into info.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t               *plugin,
                       uint32_t                           index,
                       bool                               is_input,
                       clap_audio_port_data_types_info_t *info);

   // process audio, events, ...
   // All the pointers coming from clap_process_data_type_t and its nested attributes,
   // are valid until process() returns.
   // The plugin must be prepared to receive either the normal process() call,
   // or clap_plugin_audio_ports_data_types.process(), but not both for the same audio.
   // [audio-thread & active & processing]
   clap_process_status(CLAP_ABI *process)(const struct clap_plugin       *plugin,
                                          const clap_process_data_type_t *process);

} clap_plugin_audio_ports_data_types_t;

enum clap_host_audio_ports_data_types_rescan {
   // [!active] The supported_types did change
   CLAP_AUDIO_PORTS_DATA_TYPES_RESCAN_SUPPORTED_TYPES = 1 << 0,

   // [!active] The preferred_type did change
   CLAP_AUDIO_PORTS_DATA_TYPES_RESCAN_PREFERRED_TYPE = 1 << 1,
};

typedef struct clap_host_audio_ports_data_types {
   // Checks if the host allows a plugin to change a given aspect of the audio ports definition.
   // [main-thread]
   bool(CLAP_ABI *is_rescan_flag_supported)(const clap_host_t *host, uint32_t flag);

   // Rescan the full list of audio ports according to the flags.
   // It is illegal to ask the host to rescan with a flag that is not supported.
   // Certain flags require the plugin to be de-activated.
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, uint32_t flags);
} clap_host_audio_ports_data_types_t;

#ifdef __cplusplus
}
#endif
