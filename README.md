# newt
ZX Spectrum Next network tool

Copy to dot and run as follows:

.newt [-qvw] <command> [args]

-q            quiet

-v            verbose

-w            write rtc

Commands:

ip            show ip addr

info          show esp firmware

lookup <fqdn> lookup ip for fqdn

rtc [<date> <time>]     get time from RTC
              or set with `-w rtc "dd/mm/yy" "hh:mm:ss"`

sntp [server] get time from server (use -w to set rtc)
