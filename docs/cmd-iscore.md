---
tags:
  - command
---

# /iscore

## Syntax

<!--cmd-syntax-start-->
```eqcommand
/iscore [option] [setting] | [<stat> <weight>]
```
<!--cmd-syntax-end-->

## Description

<!--cmd-desc-start-->
Sets item values, which channels to check for item links, and saves/loads configurations. No options are saved by default, the save command must be issued.
<!--cmd-desc-end-->

## Options

`(no option)`
:   If an item is on cursor, evaluate for upgrades. If no item on cursor, displays current settings.

`help`
:   Displays help text

`clear`
:   Resets all settings

`save|load`
:   Saves to the INI or loads from the INI. Remember to save your settings.

`click [Group|Raid|Guild|Any]`
:   Toggles the channel to look for item links. If no option given, shows your current click settings.

`<stat> <weight>`
:   Sets the weight of a stat. The higher the weight, the more important the stat will be to your class.
    
    **Available stats:** AC, HP, HPReg, Mana, ManaReg, hSTR, hSTA, hAGI, hDEX, hINT, hWIS, hCHR, Heal, Nuke, Clrv, Attack, Haste, Ratio

`best [value]`
:   Sets item on your cursor to the best.
