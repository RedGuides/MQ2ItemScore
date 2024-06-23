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

The configuration file MQ2ItemScore.ini is used to customize item evaluation settings. 

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



