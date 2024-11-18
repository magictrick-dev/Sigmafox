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
b32         file_copy_all(ccptr source, ccptr destination);
u64         file_last_write_time(ccptr file_path);

vptr        file_stream_handle_create(ccptr file_path, u32 file_context);
void        file_stream_handle_close(vptr handle);
void        file_stream_reset_offset(vptr handle);
void        file_stream_set_offset_rel(vptr handle, u64 relative_offset);
void        file_stream_set_offset_abs(vptr handle, u64 absolute_offset);
u64         file_stream_get_offset(vptr handle);
u64         file_stream_read(vptr handle, u64 read_size, vptr dest, u64 dest_size);
b32         file_stream_is_eof(vptr handle);

u64         file_current_working_directory(u32 buffer_size, cptr buffer);
void        file_canonicalize_path(u32 buffer_size, cptr dest, ccptr path);

#endif
