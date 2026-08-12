# CWWK CW-ADLN-SFP+

This page describes how to run coreboot on the CWWK CW-ADLN-SFP+.

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

The board designation is not exposed anywhere in software; SMBIOS reports
`Default string` for every field. According to the sticker on the board:
`CW-ADLN-SFP+`. The revision is printed underneath the SO-DIMM and reads
`CW-ADLN-SFP+ v1.3` on the boards this port was developed against.

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

### Returning to vendor firmware

CWWK publishes a bootable UEFI shell ISO containing a complete 16MiB
image and Intel's `Fpt.efi`, which writes descriptor, ME and BIOS in one
pass:

```
Fpt.efi -f <image>.bin
```

That is the simplest route back to stock, and it needs no external
programmer. Note the converse: running the vendor updater on a machine
running coreboot will silently replace it, and will also set the
descriptor soft straps to the vendor's current values, which differ in
three bytes from what these boards shipped with.

The vendor image is built on the Twin Lake FSP and covers N100, i3-N305,
N150 and N355 with a single binary. This coreboot port was developed
against i3-N305. An N150 or N355 board of the same design would want
`SOC_INTEL_TWINLAKE`, as `topton/adl` does for its own N150 variant, and
is untested here.

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

There is no external serial port. The board brings serial out on `JCOM`,
a 2x10 header at 2.0mm pitch on the left edge near the SFP+ cages.
Ordinary 2.54mm IDC cables do not fit.

`JCOM` is driven by a TI MAX3243E, so it carries **RS-232 levels, not
3.3V TTL**. Do not wire a TTL serial adapter to it.

CWWK's motherboard specification for this board gives the pinout:

```
 1 DCD_A    2 RXD_A       11 RXD_B     12 TXD_B
 3 TXD_A    4 DTR_A       13 RTS_B     14 CTS_B
 5 GND      6 DSR_A       15 n/c       16 n/c
 7 RTS_A    8 CTS_A       17 GND       18 GND
 9 RI_A    10 n/c         19 PWR_BTN#  20 RESET#
```

So COM1 appears with full modem control, COM2 with transmit, receive and
flow control only, and the power and reset buttons share the header.
The vendor prints pins 11 and 12 both as `SOUT_B_J`; 11 is receive, by
symmetry with the A port.

`JC13` and `JC14` beside it are the RS232/RS485 mode selectors, one per
port, each a 2x3 with two independent three-pin groups. The factory
position is RS-232. Leave them there unless you want RS-485.

## Firmware configuration

With an edk2 payload and `SMMSTORE`, the setup menu exposes the power
state to restore after a power loss (CFR `power_on_after_fail`).

## Functionality

### Tested and working

- Both Intel 82599ES 10GbE SFP+ ports
- Both Intel I226-V 2.5GbE ports
- All six USB ports, microSD card reader
- M.2 M key 2280 (NVMe)
- M.2 E key, wired for both PCIe and CNVi
- Intel PTT (fTPM 2.0)
- IT8625E temperature, voltage and fan control
- Serial console
- Payload: SeaBIOS, edk2

### Untested, broken

- USB over-current pins are not mapped (`OC_SKIP`). The board wires OC1
(`GPP_A14`) and OC3 (`GPP_A16`), but which port belongs to which pin has
not been measured.
- Audio. The board has a combined MIC and line-out jack driven by the
ALC897, and the verb table is derived from the vendor firmware, but the
jack has not been tested.
- S3 suspend.

## Specification

```{eval-rst}
+--------+-----------------------------------------+
| SoC    | Intel AlderLake N100/N305 (IoT)         |
+--------+-----------------------------------------+
| Memory | DDR5 SO-DIMM (single-channel)           |
+--------+-----------------------------------------+
| EC     | ITE IT8625E                             |
+--------+-----------------------------------------+
| SPI    | Winbond W25Q128 (16MiB 3.3V)            |
+--------+-----------------------------------------+
| NIC    | 2x Intel 82599ES (10GbE SFP+)           |
+--------+-----------------------------------------+
|        | 2x Intel I226-V (2.5GbE)                |
+--------+-----------------------------------------+
| Video  | 2x HDMI                                 |
+--------+-----------------------------------------+
```

[flashprog]: https://flashprog.org/wiki/Flashprog
[flashrom]: https://flashrom.org/
