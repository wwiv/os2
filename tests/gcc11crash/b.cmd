g++ -DFMT_HEADER_ONLY=1 -DPDC_WIDE -O3 -DNDEBUG -std=gnu++17  -I C:/git/wwiv/ -I C:/git/wwiv/deps/fmt/include -I C:/git/wwiv/deps/pdcurses -c -s newinit.cpp

rem (this worked with save temps) g++ -DFMT_HEADER_ONLY=1 -DPDC_WIDE -O3 -DNDEBUG -std=gnu++17  -I C:/git/wwiv/ -I C:/git/wwiv/deps/fmt/include -I C:/git/wwiv/deps/pdcurses -save-temps -c -s newinit.cpp
