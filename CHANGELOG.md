# Changelog

## Unreleased

### Added

- **gzip container support.** `gzip`, `gzip-with`, and `gunzip` compress and
  decompress using the gzip framing (RFC 1952) instead of the raw zlib stream
  format, so their output interoperates with `.gz` files, the command-line
  `gzip`, and HTTP `Content-Encoding: gzip`. They mirror `deflate`,
  `deflate-with`, and `inflate`; `gzip` of the empty string produces a valid
  empty gzip member.
