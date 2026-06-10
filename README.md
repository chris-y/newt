# newt
ZX Spectrum Next network tool

Copy to dot and run as follows:

.newt [-qv] <command> [args]

-q            quiet

-v            verbose

Commands:

ip            show ip addr

info          show esp firmware

lookup <fqdn> lookup ip for fqdn

rtc [set]     get time from RTC
              or set with `rtc set "dd/mm/yy" "hh:mm:ss"`

sntp [server] get time from server
