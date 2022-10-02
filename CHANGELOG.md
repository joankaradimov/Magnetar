# Changelog

## Unreleased
  - Add a new trigger action: "Execute Lua"
  - Fix an issue in melee unit initialization (introduced in 0.2.0)
  - Fix an issue in sprite initialization (introduced in 0.2.0)
  - Add a command line argument `-keep-app-active` that keeps the app active in the background

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
