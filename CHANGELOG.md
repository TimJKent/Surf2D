# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.0] - 2022-07-27

### Added
- Project Properties Menu
- Physics Material Asset
- Gravity Scale adjustable in Properties/Physics Menu
- Velocity Iterations adjustable in Properties/Physics Menu
- Position Iterations adjustable in Properties/Physics Menu

### Changed
- Physics Code is now moved from Scene.h to PhysicsEngine.h

### Fixed
- Selected Path now updates after deleting a file that is selected

## [2.0.3] - 2022-07-25

### Changed
- SurfLib.dll is automatically compilled and added to the correct location

### Fixed
- mono-2.0-sgen.dll is now copied to/from the correct locations

## [2.0.2] - 2022-07-23

### Added
- Sprite Scaling/Offset
- New Promotional Material

### Fixed
- SpriterRenderer FlipX now Works with animations
- Duplicating an Object now copies ALL of its components
- Fixed Crashing after running Scene more than once

## [2.0.1] - 2022-07-20
### Changed
- Moved Script Engine Code to its own class.

### Fixed
- Script Engine file paths are now relative instead of hard coded.

## [2.0.0] - 2022-07-14
### Added
- Mono Scripting

### Removed
- Lua Scripting

### Fixed
- Too many bugs to list


## [1.0.0] - 2022-01-30
### Added
  Initial Engine Release
