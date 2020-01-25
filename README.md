# zlib.carp

is a high-level wrapper around [zlib](https://zlib.net/).

## Installation

```clojure
(load "https://veitheller.de/git/carpentry/zlib.git@0.0.1")
```

## Usage

The `ZLib` module provides only two functions, `inflate` and `deflate`. These
functions work in tandem to provide you with data compression.

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

<hr/>

Have fun!
