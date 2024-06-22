# Changelog

## Develop

## v3.2.0

- Add user argument option
- Improve the variable names to better conform *LLVM* tidy config

## v3.1.0

- Preparation for `v3.1`
- Replace `size_t` with custom defined type `lwrb_sz_t` which matches atomicity requirements
- `lwrb_sz_t` is by default typedef-ed as `unsigned long`
- Prepare `lwrb_write_ex` and `lwrb_read_ex` functions
- Implement `lwrb_write_ex` and `lwrb_read_ex` functions
- Fix `_ex` module throwing an error for Platform.IO

## v3.0.0

- Added macros for optional STDATOMIC. Global `-DLWRB_DISABLE_ATOMIC` macro will disable C11 `<stdatomic.h>` functionality.
- Add `lwrb_move` and `lwrb_overwrite`
- Fix `lwrb_find` which failed to properly search for tokens at corner cases

## v3.0.0-RC1

- Split CMakeLists.txt files between library and executable
- Change license year to 2022
- Update code style with astyle
- Minimum required version is C11, with requirement of `stdatomic.h` library
- Add `.clang-format` draft

## v2.0.3

- Add `library.json` for Platform.IO

## v2.0.2

- Add `volatile` keyword to all local variables to ensure thread safety in highest optimization
- Add local variables for all read and write pointer accesses
- Remove generic `volatile` keyword from func parameter and replace to struct member

## v2.0.1

- Fix wrong check for valid RB instance
- Apply code style settings with Artistic style options
- Add thread safety docs

## v2.0.0

- Break compatibility with previous versions
- Rename function prefixes to `lwrb` instead of `ringbuff`
- Add astyle code syntax correction

## v1.3.1

- Fixed missing `RINGBUFF_VOLATILE` for event callback causes compiler warnings or errors

## v1.3.0

- Added support for events on read/write or reset operation
- Added optional volatile parameter for buffer structure
- Fix bug in skip and advance operation to return actual amount of bytes processed
- Remove `BUF_PREF` parameter and rename with fixed `ringbuff_` prefix for all functions

## v1.2.0

- Added first sphinx documentation

## v1.1.0

- Code optimizations, use pre-increment instead of post
- Another code-style fixes

## v1.0.0

- First stable release
