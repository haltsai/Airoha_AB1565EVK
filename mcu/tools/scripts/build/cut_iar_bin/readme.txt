cut_iar_bin: Cut sub-bin from IAR single full bin, v1.0.0

Background:
IAR does not support multi-bin download to flash with CMSIS-DAP interface.
Therefore, our IAR output bin is a single full bin including a BootLoader, CM4 main bin, and PWR/N9/NVKEY bin.
In some scenarios (such as OTA), user must have the cut_iar_bin.py tool to cut sub-bin from <iar_bin>.

Usage:
    cut_iar_bin <chip_type> <iar_bin_path> [offset] [length]
    Example:
    python3 <cut_iar_bin.py> 7686 <iar_bin_path>, get CM4 main bin as OTA image for default memory layout.

    Note:
    If user modifies default memory layout, please use [offset] and [length] to get your OTA image.

    Example: python3 <cut_iar_bin.py> 7687 <iar_bin> 0 0x8000.