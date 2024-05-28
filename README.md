# MQ2ItemScore Plugin Documentation

## Overview
MQ2ItemScore is a plugin for MacroQuest that evaluates if an item is an upgrade based on your class and preferences. It is a remake of the MQ2GearScore plugin.

## Features
- **Item Evaluation**: Determines if an item is an upgrade for your class.
- **Customizable Preferences**: Users can set their own criteria for item upgrades.
- **Multi-slot Evaluation**: Provides upgrade values for items that fit in multiple slots, which is particularly useful for augmentations.

## Installation
MQ2ItemScore is included in the Very Vanilla install at RedGuides. For detailed installation instructions, visit the [RedGuides community](https://www.redguides.com/community/resources/mq2itemscore.2247/).

## Usage
- **Basic Command**: `/iScore`
  - Evaluates the item on the cursor or shows current values.
- **Commands**:
  - `/iScore clear` - Resets all variables.
  - `/iScore load` - Loads settings from disk.
  - `/iScore save` - Saves current settings to disk.
  - `/iScore click` - Shows current click options.
  - `/iScore click [Group|Raid|Guild|Any] [Value]` - Toggles or sets auto click option.
  - `/iScore click Channel [Value]` - Will auto click links in this channel.
  - `/iScore report [Channel]` - Send upgrade using this command, e.g., `/bc`.
  - `/iScore [attribute] [weight]` - Sets an item weight.
  - `/iScore best [value]` - Sets item on cursor to this value.

- **Quick Start**:
  1. Load the plugin: `/plugin itemscore load`
  2. Set importance for Armor Class: `/iScore AC 10`
  3. Set importance for Hit Points: `/iScore HP 1`
  4. Set importance for Stamina: `/iScore hSTA 15`
  5. Enable click effect evaluation: `/iScore click any`
  6. Save the configuration: `/iScore save`
  7. Link any item over say or group chat to see if it's an upgrade, or hold an item on your cursor and type `/iScore`.

## Configuration Options
- **Stat Priorities**: Define which stats are most important for your class.
- **Weight Adjustments**: Set the weight for each stat to customize item evaluations.
- **Syntax**: `/iScore [option] [setting] | [<stat> <weight>]`
  - Sets item values, channels for item links, and saves/loads configurations. Options are not saved by default; the save command must be issued.

## Configuration File

The configuration file MQ2ItemScore.ini is used to customize item evaluation settings. Below is an example of the file contents:

```ini
[Global]
Report=bc
ClickChannel=
ClickGroup=
ClickGuild=
ClickRaid=
ClickAny=
[WAR]
AC=200.00
HP=10.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=50.00
hSTA=60.00
hAGI=75.00
hDEX=40.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=300.00
Shielding=300.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1000.00
[BER]
AC=5.00
HP=5.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=25.00
hSTA=5.00
hAGI=10.00
hDEX=75.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1150.00
[RNG]
AC=5.00
HP=5.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=25.00
hSTA=5.00
hAGI=10.00
hDEX=75.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1150.00
[MAG]
AC=0.00
HP=5.00
HPReg=0.00
Mana=2.00
ManaReg=0.00
hSTR=0.00
hSTA=50.00
hAGI=0.00
hDEX=0.00
hINT=50.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=250.00
Clrv=25.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=0.00
[ROG]
AC=5.00
HP=5.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=25.00
hSTA=5.00
hAGI=10.00
hDEX=75.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1150.00
[CLR]
AC=1.00
HP=6.00
HPReg=0.00
Mana=1.00
ManaReg=0.00
hSTR=0.00
hSTA=75.00
hAGI=0.00
hDEX=0.00
hINT=0.00
hWIS=1.00
hCHR=0.00
Heal=250.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=0.00
[BRD]
AC=6.00
HP=2.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=0.00
hSTA=25.00
hAGI=25.00
hDEX=150.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=10.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1000.00
[SHM]
AC=1.00
HP=6.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=0.00
hSTA=75.00
hAGI=1.00
hDEX=0.00
hINT=0.00
hWIS=1.00
hCHR=0.00
Heal=250.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1000.00
[NEC]
AC=0.00
HP=5.00
HPReg=0.00
Mana=2.00
ManaReg=0.00
hSTR=0.00
hSTA=50.00
hAGI=0.00
hDEX=0.00
hINT=50.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=250.00
Clrv=25.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=0.00
[PAL]
AC=50.00
HP=3.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=5.00
hSTA=75.00
hAGI=10.00
hDEX=75.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=1.00
Shielding=1.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1000.00
[ENC]
AC=0.00
HP=5.00
HPReg=0.00
Mana=2.00
ManaReg=0.00
hSTR=0.00
hSTA=50.00
hAGI=0.00
hDEX=0.00
hINT=50.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=250.00
Clrv=25.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=0.00
[BST]
AC=5.00
HP=5.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=25.00
hSTA=5.00
hAGI=10.00
hDEX=75.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1150.00
[WIZ]
AC=0.00
HP=5.00
HPReg=0.00
Mana=2.00
ManaReg=0.00
hSTR=0.00
hSTA=50.00
hAGI=0.00
hDEX=0.00
hINT=50.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=250.00
Clrv=25.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=0.00
[SHD]
AC=50.00
HP=3.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=5.00
hSTA=75.00
hAGI=10.00
hDEX=75.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=1.00
Shielding=1.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1000.00
[DRU]
AC=1.00
HP=6.00
HPReg=0.00
Mana=1.00
ManaReg=0.00
hSTR=0.00
hSTA=75.00
hAGI=0.00
hDEX=0.00
hINT=0.00
hWIS=1.00
hCHR=0.00
Heal=250.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=0.00
[MNK]
AC=5.00
HP=5.00
HPReg=0.00
Mana=0.00
ManaReg=0.00
hSTR=25.00
hSTA=5.00
hAGI=10.00
hDEX=75.00
hINT=0.00
hWIS=0.00
hCHR=0.00
Heal=0.00
Nuke=0.00
Clrv=0.00
Attack=0.00
Accuracy=0.00
CE=0.00
StrikeThrough=0.00
Avoidance=0.00
Shielding=0.00
DoTShielding=0.00
SpellShield=0.00
Stun=0.00
DS=0.00
Haste=0.00
Ratio=1150.00
```

## Contributing
To contribute to the development of this plugin:
1. Fork the repository on GitHub.
2. Create a new branch for your feature or bug fix.
3. Submit a pull request with a detailed description of your changes.

## Authors
- **dewey2461**: Initial work
- **Knightly1** and **brainiac**: Additional contributions

## Support
For support and more information, visit the [RedGuides community](https://www.redguides.com/community/resources/mq2itemscore.2247/).

## Additional Resources
- [Wiki Manual](https://www.redguides.com/community/resources/mq2itemscore.2247/)
- [GitHub Repository](https://github.com/RedGuides/MQ2ItemScore)



