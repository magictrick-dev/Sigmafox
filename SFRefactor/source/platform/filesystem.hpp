// --- Platform Filesystem API -------------------------------------------------
//
// Written by Chris DeJong Jan. 2023 / Northern Illinois University
//
//      The platform filesystem API provides the front-end functionality needed
//      to interact with the system's filesystem APIs and related utilities.
//      The API contains two primary I/O endpoints: full file reads and streaming.
//      Generally, full-file reads are the preferred option for trivial usecases,
//      but for more advanced functionality such as threaded-resource fetching,
//      the streaming API is more desirable. The streaming API isn't a fully
//      fleshed out API and only supports read-streams.
//
//      Path construction utilities are provided.
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_PLATFORM_FILESYSTEM_H
#define SIGMAFOX_PLATFORM_FILESYSTEM_H
#include <definitions.hpp>

b32         file_exists(ccptr file_path);
b32         file_ready(ccptr file_path);
b32         file_is_directory(ccptr file_path);
b32         file_is_file(ccptr file_path);
u64         file_size(ccptr file_path);
u64         file_read_all(ccptr file_path, vptr buffer, u64 buffer_size);
u64         file_write_all(ccptr file_path, vptr buffer, u64 buffer_size);

u64         file_current_working_directory(u32 buffer_size, cptr buffer);
void        file_canonicalize_path(u32 buffer_size, cptr dest, ccptr path);

#endif
