#include <stdio.h>
#include <stdlib.h>

/* Length-aware file I/O so tests can round-trip gzip streams (which contain
   NUL bytes) through the filesystem and the system gzip/gunzip tools without
   the truncation a strlen-based writer/reader would cause. ZLibZBytes is
   defined in zlib_helper.h, which zlib.carp includes before this header. */

int ZLibTest_write_file(String* path, ZLibZBytes b) {
  FILE* f = fopen(*path, "wb");
  if (!f) return -1;
  size_t n = fwrite(b.bytes, 1, b.len, f);
  fclose(f);
  return (int)n;
}

ZLibZBytes ZLibTest_read_file(String* path) {
  ZLibZBytes b;
  b.bytes = NULL;
  b.len = 0;
  FILE* f = fopen(*path, "rb");
  if (!f) return b;
  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (sz < 0) { fclose(f); return b; }
  char* buf = malloc(sz > 0 ? (size_t)sz : 1);
  size_t n = fread(buf, 1, (size_t)sz, f);
  fclose(f);
  b.bytes = buf;
  b.len = (int)n;
  return b;
}
