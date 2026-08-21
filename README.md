# zlib.carp

is a high-level wrapper around [zlib](https://zlib.net/).

## Installation

```clojure
(load "git@git.veitheller.de:carpentry/zlib.git@0.0.5")
```

## Usage

The `ZLib` module provides two pairs of functions: `deflate` and `inflate` for
strings, and `deflate-bytes` and `inflate-bytes` for binary data. Each pair
works in tandem to provide you with data compression.

```clojure
; deflate returns a Result of either binary data or an error message
(let [deflated (ZLib.deflate "mystring")]
  (match deflated
    ; inflate returns a Result of either a string or an error message
    (Success bin) (println* &(inflate bin))
    (Error msg) (IO.errorln &msg)))
```

Because it’s a `Result` type, we can apply combinators to it.

```clojure
(=> (ZLib.deflate "mystring")
    (Result.and-then &ZLib.inflate)
    (Result.map-error &(fn [msg] (do (println* &msg) msg)))
)
```

You can also choose different levels of compression using `deflate-with`. The
levels are defined in `ZLib.ZLevel`, and are `NoCompression`, `BestSpeed`,
`BestCompression`, and `DefaultCompression`, which is, well, the default.

## Binary data

`deflate` and `inflate` are bounded by `String` semantics: their input stops at
the first NUL byte, and the compressed payload they pass around is opaque.
Compressed data is full of NUL bytes, so if you want to write it to a file or
send it over a socket—or if what you are compressing isn’t text in the first
place—use the `(Array Byte)` API instead.

```clojure
(=> (ZLib.deflate-bytes &(String.to-bytes "mystring"))
    (Result.and-then &ZLib.inflate-bytes))
```

It offers the same three entry points, `deflate-bytes`, `deflate-bytes-with`,
and `inflate-bytes`, all of which take and return `(Array Byte)`.

<hr/>

Have fun!
