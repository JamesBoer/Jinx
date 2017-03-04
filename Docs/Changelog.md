# Change Log
All notable changes to this project will be documented in this file.

## [0.4.5] - 2017-03-04

### Fixed
- Jinxpad keyword list corrected

### Changed
- Collection elements are removed by setting value to null

### Removed
- Removed some potentially problematic library functions

## [0.4.4] - 2017-02-27

### Fixed
- Error when parsing multi-part variables
- Potential infinite loop in core library
- Potential for incorrect memory values in multi-threaded programs 

### Changed
- Improved fuzz testing program for better performance and easier debugging

## [0.4.3] - 2017-02-08
### Added
- Function name parts can now be optional

### Fixed
- Function name registration was incorrectly converting to lower case
- Function as last parameter of another function call was not being handled correctly

## [0.4.2] - 2017-01-31
### Added
- New 'external' keyword for variables set via API

### Removed
- Obsolete library functions dealing with externally set variables have been removed

### Changed
- Tutorial updated with 'external' keyword use and examples

## [0.4.1] - 2016-12-08
### Added
- Instruction count performance metric

### Changed
- Cleaned up expression parsing code
- Add Variant class documentation

### Fixed
- Bug in block parsing
- Improved documentation to better describe complex function parameters

## [0.4.0] - 2016-12-01
### Added
- New keyword 'until' used similarly to 'while' in loops and conditional wait statements, but with inverted conditional logic
- New test cases for 'until' keyword
- New 'until' keyword added to Jinx editor

### Changed
- 'yield' keyword is now 'wait', a more user-friendly term

### Removed
- Deleted unused keywords in source

## [0.3.0] - 2016-11-04
### Added
- Support for multi-word variable and property support without requiring enclosing quotes

## [0.2.2] - 2016-11-01
### Fixed
- Improved variant conversion function

### Added
- New variant conversion tests and methods
- Variant conversion unit test

## [0.2.1] - 2016-09-21
### Fixed
- Improved thread-safety of collection operators
- Fixed mutex issues on macOS and Linux

### Added
- Add new test case for collections

### Changed
- Changed OSX to macOS

## [0.2.0] - 2016-09-14
### Added
- yield while {conditional} support
- Documentation for yield while conditional
- Test cases for yield

### Fixed
- Minor documentation errors

## [0.1.3] - 2016-09-04
### Added
- Default library import API

### Fixed
- Prevent memory leaks due to circular references by collections
- Validate collection key types at runtime
- Allow initialization lists inside initialization lists

## [0.1.2] - 2016-08-14
### Added
- PDF version of tutorial and LibCore library doc

### Removed
- Overview doc removed, as it's now a bit redundant

### Changed
- Jinx now uses % operator instead of mod keyword
- Floating-point mod operations now supported

### Fixed
- Minor fixes to Variant class
- Updated documentation

## [0.1.1] - 2016-08-14
### Added
- LogAllocations() function generates a detailed report of all allocated memory

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


