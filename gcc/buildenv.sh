BUILD_DIR=/usr/local11
mkdir ${BUILD_DIR}

# make sure that the UNIX sort (in /usr/libexec/bin or elsewhere) is in the path before
# C:\OS2's sort
set -e

export CFLAGS='-O2 -g -march=pentium4'
export CXXFLAGS='-O2 -g -march=pentium4'
export LDFLAGS='-g -Zomf -Zmap -Zargs-wild -Zhigh-mem'
export LDFLAGS_FOR_TARGET="$LDFLAGS"


echo "BEGINLIBPATH (BEFORE) : $BEGINLIBPATH"

export PATH="$BUILD_DIR/gcc${PATH:+;$PATH}"
export BEGINLIBPATH="$BUILD_DIR/gcc${BEGINLIBPATH:+;$BEGINLIBPATH}"
export LIBPATHSTRICT=T

echo "PATH:         $PATH"
echo "BEGINLIBPATH: $BEGINLIBPATH"


