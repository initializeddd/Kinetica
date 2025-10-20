#ifndef KINETICA_TYPES_HPP
#define KINETICA_TYPES_HPP

#include <cstdint>

namespace Kinetica {

    // =============================================================================
    // Exit Codes
    // Used by main() to communicate program termination status to the OS.
    // Follows Unix convention: 0 = success, 1–125 = application-defined errors.
    // =============================================================================
    enum class EExitCode : int
    {
        Success           = 0,  ///< Program completed successfully
        InvalidArguments  = 1,  ///< Invalid command-line arguments or usage
        InitializationFailed = 2, ///< Critical system (e.g., window, input) failed to initialize
        GpuBackendError   = 3,  ///< Custom GPU backend failed to start or operate
        FileAccessError   = 4,  ///< File not found, permission denied, or I/O failure
        UserCancelled     = 5,  ///< User explicitly quit during startup or operation
        PluginLoadError   = 6,  ///< Failed to load a required or user-specified plugin
        RenderFailure     = 7,  ///< Rendering pipeline encountered an unrecoverable error
    };

    // =============================================================================
    // Type Aliases
    // =============================================================================
    using u8  = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using i8  = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
    using f32 = float;
    using f64 = double;

    // =============================================================================
    // Forward Declarations
    // =============================================================================

} // namespace Kinetica

#endif // KINETICA_TYPES_HPP
