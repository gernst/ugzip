# ugzip
Small gzip implementation
# Small gzip/ungzip

`ugzip` is a small, non-POSIX compatible alternative to [GNU gzip](http://www.gnu.org/software/gzip/gzip.html).

It depends on [zlib](http://www.zlib.net/) and [lowfat](http://www.fefe.de/libowfat/).

The GNU gzip 1.4 fails to build with dietlibc because of its dependency on the
internal structure of FILE in e.g. lib/freadahead.c.

Gzip 1.2.4 compiles with dietlibc, but it
*may crash when an input file name is too long (over 1020 characters)*.

`ugzip` supports the following options:

    -c, --stdout      write on standard output, keep original files unchanged
    -d, --decompress  decompress
    -f, --force       force overwrite of output file and compress links
    -q, --quiet       suppress all warnings
    -S, --suffix=SUF  use suffix SUF on compressed files (only in the -S=SUF form)
    -v, --verbose     verbose mode
    -1, --fast        compress faster
    -9, --best        compress better

Running it as `[u]gunzip` will act like `[u]gzip -d`.

# Downloads

- Initial version [ugzip.tar.gz](/ugzip.tar.gz), alpha quality.
