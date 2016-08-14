# Change Log
All notable changes to this project will be documented in this file.

## [0.1.0] - 2016-08-12
### Added
- Fuzzing test to ensure runtime robustness
- New unit tests for incrementing/decrementing properties
- Bytecode execution count limiter and behavior flag

### Changed
- Modified changelog date format to year-month-date as intended

### Fixed
- Increment and decrement keywords now work correctly with properties
- Memory shutdown function fixed
- Fixed several runtime crashes or asserts from initial fuzz tests

## [0.0.4] - 2016-07-29
### Added
- WPF/.NET Editor control for Jinx
- JinxPad sample project demonstrating use of editor control

### Changed
- Reorganized library projects

## [0.0.3] - 2016-07-17
### Added
- Makefiles and NetBeans projects for Linux platform

### Fixed
- Projects compile cleanly under Linux/g++

## [0.0.2] - 2016-07-09
### Added
- Support for UTF-16 conversions in Variant class
- Added unit tests for Unicode conversion and variable names

### Fixed
- Potention memory corruption bug when using Jinx::Shutdown() and not all memory has been freed
- Identifiers now have fewer naming restrictions

### Changed
- Updated tutorial with new Unicode and naming rules
- Regenerated API documentation
- Renamed "callback" unit test to "native"

### Removed
- Deleted unused opcode
- Removed case insensitivity

## [0.0.1] - 2016-07-08
### Added
- Initial release
- Added initial library version numbers and string
- Added changelog

### Fixed
- Minor documentation issues


