export CFLAGS='-O2 -g -march=pentium4'
export CXXFLAGS='-O2 -g -march=pentium4'
export LDFLAGS='-g -Zomf -Zmap -Zargs-wild -Zhigh-mem'
export LDFLAGS_FOR_TARGET="$LDFLAGS"

./configure --prefix=/usr/local11 \
  --enable-shared --enable-languages=c,c++ \
  --with-gnu-as --enable-frame-pointer \
  --disable-bootstrap --disable-libstdcxx-pch \
  --enable-threads --enable-decimal-float=yes \
  --build=i686-pc-os2-emx --target=i686-pc-os2-emx
