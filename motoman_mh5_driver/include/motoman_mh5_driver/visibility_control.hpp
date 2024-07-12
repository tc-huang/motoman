#ifndef MOTOMAN_MH5_DRIVER__VISIBILITY_CONTROL_HPP_
#define MOTOMAN_MH5_DRIVER__VISIBILITY_CONTROL_HPP_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define MOTOMAN_MH5_DRIVER_EXPORT __attribute__((dllexport))
#define MOTOMAN_MH5_DRIVER_IMPORT __attribute__((dllimport))
#else
#define MOTOMAN_MH5_DRIVER_EXPORT __declspec(dllexport)
#define MOTOMAN_MH5_DRIVER_IMPORT __declspec(dllimport)
#endif
#ifdef MOTOMAN_MH5_DRIVER_BUILDING_DLL
#define MOTOMAN_MH5_DRIVER_PUBLIC MOTOMAN_MH5_DRIVER_EXPORT
#else
#define MOTOMAN_MH5_DRIVER_PUBLIC MOTOMAN_MH5_DRIVER_IMPORT
#endif
#define MOTOMAN_MH5_DRIVER_PUBLIC_TYPE MOTOMAN_MH5_DRIVER_PUBLIC
#define MOTOMAN_MH5_DRIVER_LOCAL
#else
#define MOTOMAN_MH5_DRIVER_EXPORT __attribute__((visibility("default")))
#define MOTOMAN_MH5_DRIVER_IMPORT
#if __GNUC__ >= 4
#define MOTOMAN_MH5_DRIVER_PUBLIC __attribute__((visibility("default")))
#define MOTOMAN_MH5_DRIVER_LOCAL __attribute__((visibility("hidden")))
#else
#define MOTOMAN_MH5_DRIVER_PUBLIC
#define MOTOMAN_MH5_DRIVER_LOCAL
#endif
#define MOTOMAN_MH5_DRIVER_PUBLIC_TYPE
#endif

#endif  // MOTOMAN_MH5_DRIVER__VISIBILITY_CONTROL_HPP_