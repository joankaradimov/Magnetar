# Changelog

## Unreleased
  - Reimplement ~15% of the functions in StarCraft 1.16.1
  - Use iscript animations from a source file instead of a binary
  - Fix crashes caused by long strings (e.g. scenario title/description)
  - Fix a crash in mission briefing initialization
  - Add a new trigger action: "Execute Lua"
    * Implement a `Scenario` type
    * Implement a `Player` type
    * Implement a `Race` type
    * Implement a global `print` function
    * Implement a global `require` function

## 0.3.0 - Committed Corsair
  - Allow raw CHK files to be loaded as custom scenarios
  - Add command line arguments:
    * `-keep-app-active` - keep the app active while in the background
    * `-skip-end-mission-prompt` - remove the win/lose dialog upon mission completion
    * `-help` - display a help message
  - Hide game type UI elements for directories during custom scenario selection
  - Attempt to auto-detect the StarCraft installation directory
  - Fix issues with buildability flags in the bottom map corners (introduced in 0.1.0)
  - Fix an issue in units updaing within the game loop (introduced in 0.2.0)
  - Fix an issue in melee unit initialization (introduced in 0.2.0)
  - Fix an issue in sprite initialization (introduced in 0.2.0)

## 0.2.0 - Busy Barracks
  - Reimplement ~10% of the functions in StarCraft 1.16.1
  - Allow arbitrary map sizes below 256x256
  - Draw a higher details minimap for map sizes below 32x32
  - Put the Precursor campaign on the campaigns menu screen
  - Implement new tags for credits/establishing shots:
    * `</PLAYSOUND xxx>`
    * `</SCREENUPPERLEFT>`
    * `</SCREENUPPERRIGHT>`
    * `</SCREENLOWERRIGHT>`
    * `</FONTSIZE xxx>` - where xxx is `10`, `14`, `16` or `16x` (default)
  - Fix wrong colors in magma/water tiles on Ashworld/Ice tilesets (by DarkenedFantasies)
  - Allow mouse scrolling in the campaign history popup
  - Increase the frequency of minimap rerendering

## 0.1.0 - Anxious Arbiter
  - Enable establishing shots within custom scenarios
