/*
  zip_source_file_win32_ansi.c -- source for Windows file opened by ANSI name
  Copyright (C) 1999-2020 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <libzip@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.
  3. The names of the authors may not be used to endorse or promote
  products derived from this software without specific prior
  written permission.

  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "zip_source_file_win32.h"

static char *ansi_allocate_tempname(const char *name, size_t extra_chars, size_t *lengthp);
static void ansi_make_tempname(char *buf, size_t len, const char *name, zip_uint32_t i);

/* clang-format off */

// fix MSVC warning C4232 : nonstandard extension used : address of dllimport '_strdup' is not static, identity not guaranteed
// etc.etc.

static HANDLE __stdcall create_file_a(const void* name, DWORD access, DWORD share_mode, PSECURITY_ATTRIBUTES security_attributes, DWORD creation_disposition, DWORD file_attributes, HANDLE template_file)
{
	return CreateFileA(name, access, share_mode, security_attributes, creation_disposition, file_attributes, template_file);
}

static BOOL __stdcall delete_file_a(const void* name)
{
	return DeleteFileA(name);
}

static DWORD __stdcall get_file_attributes_a(const void* name)
{
	return GetFileAttributesA(name);
}

static BOOL __stdcall get_file_attributes_ex_a(const void* name, GET_FILEEX_INFO_LEVELS info_level, void* information)
{
	return GetFileAttributesExA(name, info_level, information);
}

static BOOL __stdcall move_file_a(const void* from, const void* to, DWORD flags)
{
	return MoveFileExA(from, to, flags);
}

static BOOL __stdcall set_file_attributes_a(const void* name, DWORD attributes)
{
	return SetFileAttributesA(name, attributes);
}

static char* string_duplicate_a(const char* string)
{
	return strdup(string);
}


zip_win32_file_operations_t ops_ansi = {
    ansi_allocate_tempname,
    create_file_a,
    delete_file_a,
    get_file_attributes_a,
    get_file_attributes_ex_a,
    ansi_make_tempname,
    move_file_a,
    set_file_attributes_a,
    string_duplicate_a
};

/* clang-format on */

ZIP_EXTERN zip_source_t *
zip_source_win32a(zip_t *za, const char *fname, zip_uint64_t start, zip_int64_t len) {
    if (za == NULL)
        return NULL;

    return zip_source_win32a_create(fname, start, len, &za->error);
}


ZIP_EXTERN zip_source_t *
zip_source_win32a_create(const char *fname, zip_uint64_t start, zip_int64_t length, zip_error_t *error) {
    if (fname == NULL || length < -1) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    return zip_source_file_common_new(fname, NULL, start, length, NULL, &_zip_source_file_win32_named_ops, &ops_ansi, error);
}


static char *
ansi_allocate_tempname(const char *name, size_t extra_chars, size_t *lengthp) {
    *lengthp = strlen(name) + extra_chars;
    return (char *)malloc(*lengthp);
}


static void
ansi_make_tempname(char *buf, size_t len, const char *name, zip_uint32_t i) {
    snprintf(buf, len, "%s.%08x", name, i);
}
