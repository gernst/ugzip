#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>

#include <buffer.h>
#include <str.h>

#include <zlib.h>

#define BUFSIZE 8192
static char buf[BUFSIZE];
static char *prog;

static int c,d,f,q,K=Z_DEFAULT_COMPRESSION;
static const char *S="gz";

static void die(const char *msg) {
    if(!q) {
        buffer_puts(buffer_2, prog);
        buffer_puts(buffer_2, ": ");
        buffer_puts(buffer_2, msg);
        buffer_putnlflush(buffer_2);
    }
    exit(1);
}

static int zip(int fd0, int fd1) {
    ssize_t total = 0;

    gzFile out = gzdopen(fd1, "wb");

    if(!out) die("gzdopen failed for output");
    if(gzsetparams(out, K, Z_DEFAULT_STRATEGY) == Z_STREAM_ERROR)
        die("gzsetparams failed for output");

    do {
        ssize_t len0 = read(fd0, buf, sizeof(buf));
        if(len0<0) die("read error");
        if(len0==0) break;
        total += len0;
        ssize_t len1 = gzwrite(out, buf, len0);
        if(len0 != len1) die("write error");
    } while(1);

    /* also flushes */
    gzclose(out);

    return total;
}

static int unzip(int fd0, int fd1) {
    ssize_t total = 0;

    gzFile in = gzdopen(fd0, "rb");

    if(!in) die("gzdopen failed for input");

    do {
        ssize_t len0 = gzread(in, buf, sizeof(buf));
        if(len0<0) die("read error");
        if(len0==0) break;
        total += len0;
        ssize_t len1 = write(fd1, buf, len0);
        if(len0 != len1) die("write error");
    } while(1);

    gzclose(in);

    return total;
}

static int usage() {
    buffer_puts(buffer_2, "usage: ");
    buffer_puts(buffer_2, prog);
    buffer_puts(buffer_2, ": ");
    buffer_puts(buffer_2, "[-cdfqv19] [-S=<suffix>] [FILES]");
    buffer_putnlflush(buffer_2);
    return 1;
}

static int gzip(const char *iname) {
    int fd0, fd1;

    if(!iname) fd0 = 0;
    else       fd0 = open(iname,O_RDONLY);

    if(fd0<0) die("could not open input");

    if(c || !iname) fd1 = 1;
    else {
        char oname[str_len(iname) + str_len(S) + 2]; /* . and \0 */

        if(d) {
            size_t at = str_copy(oname, iname) - str_len(S)-1;
            if(at<=0 || oname[at]!='.' || !str_equal(oname+at+1, S))
                die("unknown suffix");
            oname[at] = 0;
        }
        else {
            size_t at = str_copy(oname, iname);
            oname[at] = '.';
            str_copy(oname+at+1, S);
        }

        struct stat s;
        if(!f && !stat(oname, &s))
            die("output file exists (use -f to overwrite)");
        fd1 = open(oname,O_WRONLY|O_CREAT,0644);
    }

    if(fd1<0) die("could not open output");
    if(!f && !c && isatty(fd1)) die("not writing compressed data to terminal (use -f to override)");

    if(d) unzip(fd0, fd1);
    else  zip(fd0, fd1);

    if(!c && iname) unlink(iname);
}

int main(int argc, char *argv[]) {
    int i;

    prog = argv[0];
    prog += str_len(prog);
    
    if (str_equal(prog-6, "gunzip")) { d=1; }
    if (str_equal(prog-4, "zcat"))   { d=1; c=1; }

    for (i=1; i<argc; ++i) {
        if (argv[i][0]=='-') {
            int j;
            if (str_equal(argv[i],"--stdout")) c=1; else
            if (str_equal(argv[i],"--decompress")) d=1; else
            if (str_equal(argv[i],"--force")) f=1; else
            if (str_equal(argv[i],"--quiet")) q=1; else
            if (str_equal(argv[i],"--verbose")) q=0; else
            if (str_start(argv[i],"--suffix=")) S=argv[i]+9; else
            if (str_start(argv[i],"-S=")) S=argv[i]+3; else
            if (str_equal(argv[i],"--best")) K=9; else
            if (str_equal(argv[i],"--fast")) K=1; else
            if ((argv[i][1]=='-') && !argv[i][2]) { argv[i]=0; break; } else
            for (j=1; argv[i][j]; ++j) {
                switch (argv[i][j]) {
                    case 'c': c=1; break;
                    case 'd': d=1; break;
                    case 'f': f=1; break;
                    case 'q': q=1; break;
                    case 'v': q=0; break;
                    case '1': case '2': case '3': case '4': case '5':
                    case '6': case '7': case '8': case '9':
                              K = argv[i][j] - '1'; 
                              break;
                    default: return usage();
                }
            }
            argv[i]=0;
            if (j==1) break;
        }
    }

    int n=0;
    for (i=1; i<argc; ++i)
        if (argv[i]) {
            gzip(argv[i]);
            n++;
        }

    if(!n) {
        gzip(0);
    }

    return 0;
}
