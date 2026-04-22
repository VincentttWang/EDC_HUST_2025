# NUEDC2025 MSPM0 CCS Project

This project is a refactored, CCS-oriented layout extracted from `2025E/工程文件`.

## Goals
- Keep only source/config files required for development
- Remove generated outputs and one-off example test files
- Preserve module layering:
  - `drivers/BSP`: board support drivers
  - `drivers/Utils`: control, mode, and algorithm utilities
- Support direct import in CCS from `.projectspec`

## Quick Import (CCS)
1. Open CCS.
2. Select `Project -> Import CCS Projects...`.
3. Point to folder: `refactor/Project/NUEDC2025_MSPM0_CCS/ticlang`.
4. Choose project `NUEDC2025_MSPM0G3507_nortos_ticlang` and import.
5. Build `Debug` configuration.

## Notes
- Main entry file is `app/main.c`.
- SysConfig file is `config/empty.syscfg`.
- Generated config files are in `config/ti_msp_dl_config.c` and `config/ti_msp_dl_config.h`.
