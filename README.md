# zlib.carp

is a high-level wrapper around [zlib](https://zlib.net/).

## Installation

```clojure
(load "git@git.veitheller.de:carpentry/zlib.git@0.0.3")
```

## Usage

The `ZLib` module provides `inflate` and `deflate`, which work in tandem to
provide you with data compression using the raw zlib stream format.

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

For the gzip container format (RFC 1952) used by `.gz` files and HTTP
`Content-Encoding: gzip`, use `gzip` and `gunzip` instead. They behave just
like `deflate` and `inflate` (including a `gzip-with` that takes a `ZLevel`),
but emit and read the gzip framing, so their output interoperates with tools
like the command-line `gzip`.

```clojure
; gzip returns a Result of either gzip-framed binary data or an error message
(=> (ZLib.gzip "mystring")
    (Result.and-then &ZLib.gunzip)  ; gunzip is the inverse of gzip
    (Result.map-error &(fn [msg] (do (println* &msg) msg)))
)
```

<hr/>

Have fun!
