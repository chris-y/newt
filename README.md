# newt

*ZX Spectrum Next network tool*

Ensure the binary is copied to your `C:dot` directory as `newt`.

---

## Synopsis

    .newt [-qvw] <command> [args]

---

## Options

| Option | Description |
| :----- | :---------- |
| `-q`   | Quiet mode – suppress unnecessary output. |
| `-v`   | Verbose mode – display detailed diagnostic information. |
| `-w`   | Write the retrieved time to the RTC (used in combination with the `sntp` command). |

---

## Commands

| Command                                 | Description |
| :-------------------------------------- | :---------- |
| `ip`                                    | Show the current IP address. |
| `info`                                  | Display the ESP firmware version. |
| `lookup <fqdn>`                         | Perform a DNS lookup for the specified fully qualified domain name. |
| `rtc`                                   | Display the current date and time from the RTC. |
| `rtc "dd/mm/yy" "hh:mm:ss"`             | Manually set the RTC to the given date and time. |
| `sntp <offset> [server]`                | Fetch the current time from an SNTP server (uses a default if no server is provided). Offset must be specified in minutes ±UTC. Add the `-w` flag to automatically write the fetched time to the RTC (e.g., `.newt -w sntp -60 pool.ntp.org`). |
