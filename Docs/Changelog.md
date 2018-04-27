# Change Log
All notable changes to this project will be documented in this file.

## [0.15.5] - 2018-04-27

## Changed
- Version string is acquired via a function instead of being a constant

## [0.15.4] - 2018-04-18

## Added
- Added 'get call stack' core library function

## [0.15.3] - 2018-04-17

## Changed
- Improved runtime performance of local variable lookups

## [0.15.2] - 2018-02-22

## Fixed
- Fixed a bug related to function chains

## [0.15.1] - 2018-02-03

## Fixed
- Fixed a bug when assigning collection elements within a collection

## [0.15.0] - 2017-12-26

## Changed
- Function precedence now works left to right
- Compound expressions are no longer a problem for the parser

## [0.14.0] - 2017-12-06

## Changed
- Opcode debug output now shows symbol and function names
- Logical 'not' operation now has same precedence as 'or/and'
- Mod operators now works like Python instead of C++

## Fixed
- Parser now rejects function parameter names that collide with properties

## [0.13.0] - 2017-12-01

## Changed
- Logical operators now perform short-circuit evaluation
- Updated Catch unit test library

## [0.12.0] - 2017-11-19

## Changed
- Most operators no longer perform automatic type coersion, and errors are more explicitly handled.
- Documented type coersion behavior in tutorial.

## Fixed
- Fixed comparison between integer and numeric types

## [0.11.0] - 2017-11-07

## Added
- Added per-script user context pointer for native library functions.

## Fixed
- Made changes to reduced probability of name collisions.

## [0.10.3] - 2017-10-25

## Fixed
- Fix potential error in allocator with static initialization ordering

## [0.10.2] - 2017-10-13

## Fixed
- Minor change for Xcode 9 compatibility

## [0.10.1] - 2017-08-26

## Fixed
- Improved local script variable lookup speed
- Fixed potential crash on shutdown in allocator

## [0.10.0] - 2017-08-24

### Added
- New ellipse operator allows explicit continuation of statements on a new line
- Initialization lists can handle new lines between items in the list

## Fixed
- Check for name collision of variables assigned in loop statements

## [0.9.0] - 2017-08-17

### Added
- Test case for case-folding variable rules
- Total performance time interval to performance data

### Changed
- Optimized allocator for better multi-threaded performance
- Optimized script runtime to avoid unneeded case folding on internal function calls
- PerfTest and FuzzTest now use standard C++ threads for test code

## [0.8.0] - 2017-07-09

### Added
- Index operators can now be used directly following function calls for returned collections
- New performance metric for completed scripts
- New performance test
- New initialization parameter for caching memory blocks

### Changed
- Math operations now use C-style precedence rules
- Function return values are now always optional

## [0.7.0] - 2017-07-07

### Changed
- Added better support for alternative string character encodings / types
- Changed function registration with more explicit parameters
- Changed required ordering of readonly parameter for properties

## [0.6.0] - 2017-06-02

### Changed
- Implemented Unicode-aware case folding algorithm to make Jinx case insensitive

### Fixed
- Runtime type conversion errors properly checked, and unit tests added

## [0.5.0] - 2017-03-08

### Added
- Added erase keyword for removing collection elements

### Changed
- Variable and property assignment now uses set {var} to {expression} pattern
- Collection iterators now store an iterator/collection pair

### Fixed
- Multi-part variable names can include keywords as second or later parts

### Removed
- Removed obsolete library functions to erase collection element

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
- Potential memory corruption bug when using Jinx::Shutdown() and not all memory has been freed
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


