# NUEDC2025 MSPM0 CCS Project

This project is a refactored, CCS-oriented layout extracted from `2025E/工程文件`.

## Goals
- Keep only source/config files required for development
- Remove generated outputs and one-off example test files
- Preserve module layering:
  - `drivers/BSP`: board support drivers
  - `drivers/Utils`: control, mode, and algorithm utilities
- Support direct import in CCS from `.project/.cproject/.ccsproject`

## Quick Import (CCS)
1. Open CCS.
2. Select `Project -> Import Existing CCS Eclipse Project`.
3. Point to folder: `refactor/Project/NUEDC2025_MSPM0_CCS`.
4. Choose project `NUEDC2025_MSPM0G3507_nortos_ticlang` and import.
5. Build `Debug` configuration.

## Optional Import (projectspec)
If you prefer `Import CCS Projects`, use `refactor/Project/NUEDC2025_MSPM0_CCS/ticlang`.

## Why old imports looked like 2025E
CCS `projectspec` import may create a generated project copy in a different folder depending on wizard options.
This repository now includes native CCS metadata (`.project/.cproject/.ccsproject`) under this folder to avoid that behavior.

## Notes
- Main entry file is `app/main.c`.
- SysConfig file is `config/empty.syscfg`.
- Generated config files are in `config/ti_msp_dl_config.c` and `config/ti_msp_dl_config.h`.

## Build Switches
- File: `config/project_build_config.h`
- `PROJECT_ENABLE_TEST_MODES`:
  - `1`: include test menu entries in UI tree
  - `0`: exclude test menu entries for competition/release firmware
