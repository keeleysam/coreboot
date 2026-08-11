# CWWK CW-ADLN-6L

This page describes how to run coreboot on the CWWK CW-ADLN-6L.

```{eval-rst}
+---------------+---------------------------------+----------------------+
| Binary file   | Apply                           | Required / Optional  |
+===============+=================================+======================+
| IFD           | Intel Flash Descriptor          | Required (see below) |
+---------------+---------------------------------+----------------------+
| ME            | Intel Management Engine         | Required (see below) |
+---------------+---------------------------------+----------------------+
| FSP-M & FSP-S | Intel Firmware Support Package  | Required             |
+---------------+---------------------------------+----------------------+
```

## Flashing coreboot

**WARNING:** There are multiple devices from the same vendor with
similar name, using different board layout, capabilities and EC/SuperIO.

Unlike most boards from this vendor, this one identifies itself: SMBIOS
reports manufacturer `CWWK` and product `CW-ADLN-6L`. The vendor sells it
as `N305-6L`.

Please **DO NOT cross-flash firmware on "similar-looking" device**.
Doing so **may kill your device**.

### Internally

The descriptor, BIOS and ME regions are all read-write from the host, so
internal programmers such as [flashrom]/[flashprog] can write the board
without opening the case. The SPI opcode menu is locked down, so they
fall back to hardware sequencing.

Only the `bios` region needs to be written, so there is no need to
extract the `ifd` or `me` regions from your backup of the stock firmware:
`flashprog -p internal --ifd -i bios -w ./build/coreboot.rom -N`

The `-N` tells [flashprog] to skip verification on the other regions of
the flash chip which are not being written.

Do not write a full image. With coreboot running, the protected range
registers cover the descriptor and ME. A full-chip write can still erase
parts of that range but not program it again, which leaves the reset
vector blank.

### Externally

Vendor populated this board with a Winbond W25Q128.V chip in SOIC-8
package. Please note that SPI voltage on this board is standard 3.3V,
despite using mobile SoC.

Flashing coreboot using an external programmer is exactly the same as
using an internal programmer, other than the `programmer` argument:
`flashprog -p <programmer> --ifd -i bios -w ./build/coreboot.rom -N`

## Graphics

The board is configured for `NO_GFX_INIT`, so coreboot performs no
display initialization and there is no boot-time picture. Both HDMI
outputs work once the OS driver takes over. A VBT is supplied so the
driver configures the DDIs correctly.

`RUN_FSP_GOP` does not work: the Alder Lake-N IoT FSP-S does not embed a
GOP driver, so FSP produces no graphics hand-off block.

An edk2 payload can provide its own GOP for a boot-time picture using a
user-supplied `IntelGopDriver.efi`, which coreboot does not ship:

```
CONFIG_EDK2_GOP_DRIVER=y
CONFIG_EDK2_GOP_FILE="/path/to/IntelGopDriver.efi"
```

## Serial console

There is no external serial port. The board has two internal UARTs:
COM1 (`0x3f8`) on the IT8613E, and PCH SerialIO UART0, which the vendor
firmware describes in its DBG2 table as a memory-mapped 16550. Both are
routed: `GPP_H10` and `GPP_H11` are in `UART0_RXD` and `UART0_TXD`
native mode.

Two 2x5 headers at 2.0mm pitch sit near the buzzer. One is silkscreened
`GPIO1`. The other has a SIPEX SP213EEA beside it, so it carries
**RS-232 levels, not 3.3V TTL**; do not wire a TTL serial adapter to it.
Which UART reaches that header has not been measured.

## Power state after power loss

The rear panel has an `ON` / `OFF` slide switch, wired to the
`PWRON_AUTO` header on the board, which selects whether the machine
powers on by itself when mains returns. This is independent of anything
coreboot configures.

## Functionality

### Tested and working

Nothing yet. This port has not been run on hardware.

### Untested

- Everything below is derived from a probe of the vendor firmware, not
from a coreboot boot.
- All six Intel I226-V 2.5GbE ports.
- All six USB ports, and the microSD card reader.
- Both M.2 M key 2280 slots.
- Both HDMI outputs.
- Intel PTT (fTPM 2.0).
- IT8613E temperature and fan control. The fan header is FAN2 and the
CPU temperature reaches the Super I/O over PECI on TMPIN1; both were
confirmed against the vendor firmware, but the coreboot fan curve has
not been run.
- USB over-current pins are not mapped (`OC_SKIP`). The board wires OC1
(`GPP_A14`) and OC3 (`GPP_A16`), but which port belongs to which pin has
not been measured.
- The PCIe clock source assignment. The board routes seven of the ten
ADL-N `SRCCLKREQ` signals for eight root ports, and which source belongs
to which port has not been measured. See the comment in
`devicetree_6l.cb`.
- S3 suspend.

## Specification

```{eval-rst}
+--------+-----------------------------------------+
| SoC    | Intel AlderLake N100/N305 (IoT)         |
+--------+-----------------------------------------+
| Memory | DDR5 SO-DIMM (single-channel)           |
+--------+-----------------------------------------+
| EC     | ITE IT8613E                             |
+--------+-----------------------------------------+
| SPI    | Winbond W25Q128 (16MiB 3.3V)            |
+--------+-----------------------------------------+
| NIC    | 6x Intel I226-V (2.5GbE)                |
+--------+-----------------------------------------+
| Video  | 2x HDMI                                 |
+--------+-----------------------------------------+
| Audio  | HDMI only, no analog codec              |
+--------+-----------------------------------------+
```

[flashprog]: https://flashprog.org/wiki/Flashprog
[flashrom]: https://flashrom.org/
