#include "zlib.h"

// --- BEGIN ZRES HELPER ---

typedef struct {
  int len;
  char* bytes;
} ZLibZBytes;

typedef struct {
  int which;
  union {
    int err;
    ZLibZBytes* out;
  };
} ZRes;
#define ZRES_OK 0
#define ZRES_ERR 1

bool ZRes_is_ok(ZRes* r) {
  return r->which == ZRES_OK;
}

ZLibZBytes ZRes_bytes(ZRes r) {
  assert(r.which == ZRES_OK);
  return *r.out;
}

String ZRes_str(ZRes r) {
  assert(r.which == ZRES_OK);
  String res = r.out->bytes;
  free(r.out);
  return res;
}

static const char* errors[9] = {
  "Version Error",
  "Buffer Error",
  "Memory Error",
  "Data Error",
  "Stream Error",
  "errno",
  NULL,
  "Stream Ended",
  "Need Dict",
};

char* ZRes_err(ZRes r) {
  assert(r.which == ZRES_ERR);
  // +6 because zlib errors are contiguous, starting at -6
  char* x = (char*)errors[(r.err)+6];
  return String_copy(&x);
}

// --- END ZRES HELPER ---

// this code is mostly from the zlib usage example: https://www.zlib.net/zlib_how.html

#define CHUNK 16384

#define min(a,b) \
 ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
   _a < _b ? _a : _b; })

ZRes ZLib_inflate_c(ZLibZBytes b) {
  int ret;
  ZRes res;
  unsigned have;
  z_stream strm;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];
  int offs = 0;
  int len = b.len;
  char* source = b.bytes;
  ZLibZBytes* bytes = malloc(sizeof(ZLibZBytes));
  bytes->bytes = NULL;
  bytes->len = 0;

  /* allocate inflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  if (ret != Z_OK) goto err;

  /* decompress until deflate stream ends or end of file */
  do {
    strm.avail_in = min(CHUNK, len-offs);
    if (strm.avail_in <= 0) break;
    memcpy(in, source+offs, strm.avail_in);
    offs += strm.avail_in;
    strm.next_in = in;
    /* run inflate() on input until output buffer not full */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);
      assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
      switch (ret) {
      case Z_NEED_DICT:
          ret = Z_DATA_ERROR;     /* and fall through */
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
          (void)inflateEnd(&strm);
          goto err;
      }

      have = CHUNK - strm.avail_out;
      bytes->bytes = realloc(bytes->bytes, (bytes->len)+have);
      memcpy((bytes->bytes)+(bytes->len), out, have);
      bytes->len += have;
    } while (strm.avail_out == 0);
    /* done when inflate() says it’s done */
  } while (ret != Z_STREAM_END);

  /* clean up and return */
  (void)inflateEnd(&strm);
  if (ret == Z_STREAM_END) {
    res.which = ZRES_OK;
    res.out = bytes;
    return res;
  }

  /* we didn’t succeed, we fail */
  ret = Z_DATA_ERROR;
err:
  res.which = ZRES_ERR;
  res.err = ret;
  if (bytes->bytes) free(bytes->bytes);
  free(bytes);
  return res;
}

ZRes ZLib_deflate_c(String* s, int level) {
  int ret, flush;
  unsigned have;
  z_stream strm;
  ZRes res;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];
  int offs = 0;
  int len = strlen(*s);
  ZLibZBytes* bytes = malloc(sizeof(ZLibZBytes));
  bytes->bytes = NULL;
  bytes->len = 0;

  /* allocate deflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  ret = deflateInit(&strm, level);
  if (ret != Z_OK) goto err;

  /* compress until end of file */
  do {
    strm.avail_in = min(CHUNK, len-offs);
    if (strm.avail_in <= 0) break;
    memcpy(in, (*s)+offs, strm.avail_in);
    offs += strm.avail_in;
    flush = offs >= len-1 ? Z_FINISH : Z_NO_FLUSH;
    strm.next_in = in;

    /* run deflate() on input until output buffer not full, finish
       compression if all of source has been read in */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = deflate(&strm, flush);    /* no bad return value */
      assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

      have = CHUNK - strm.avail_out;
      bytes->bytes = realloc(bytes->bytes, (bytes->len)+have);
      memcpy((bytes->bytes)+(bytes->len), out, have);
      bytes->len += have;
    } while (strm.avail_out == 0);
    assert(strm.avail_in == 0);     /* all input will be used */
    /* done when last data in file processed */
  } while (flush != Z_FINISH);
  assert(ret == Z_STREAM_END);        /* stream will be complete */

  /* clean up and return */
  (void)deflateEnd(&strm);
  res.which = ZRES_OK;
  res.out = bytes;
  return res;
err:
  res.which = ZRES_ERR;
  res.err = ret;
  if (bytes->bytes) free(bytes->bytes);
  free(bytes);
  return res;
}
