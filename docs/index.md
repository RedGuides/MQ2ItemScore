---
tags:
  - plugin
resource_link: "https://www.redguides.com/community/resources/mq2itemscore.2247/"
support_link: "https://www.redguides.com/community/threads/mq2itemscore.79139/"
repository: "https://github.com/RedGuides/MQ2ItemScore"
config: "MQ2ItemScore.ini"
authors: "dewey2461"
tagline: "Reports if an item is an upgrade for your class, depending on your preferences"
quick_start: "https://www.redguides.com/wiki/MQ2ItemScore#Quick_Start"
acknowledgements: "MQ2GearScore, ieatacid and DKAA for help getting the plugin working"
---

# MQ2ItemScore

<!--desc-start-->
After assigning importance to a stat, itemscore will show you the upgrade potential of any armor that's either linked or on your cursor. 
<!--desc-end-->
If the item fits in more than one slot, you'll get an upgrade value for each slot. This is especially helpful for augs!

MQ2ItemScore is a remake of MQ2GearScore, also authored by dewey2461.

## Commands

<a href="cmd-iscore/">
{% 
  include-markdown "projects/mq2itemscore/cmd-iscore.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2itemscore/cmd-iscore.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2itemscore/cmd-iscore.md') }}

## Settings

Example MQ2ItemScore.ini, remember there are no default stats and this plugin will not do anything without configuration.

```ini
[Global]
Report=/dgt
; If an upgrade is found, where to report the good news
ClickChannel=
; Other channel to check items from
ClickGroup=0
; Check "group" chat's linked items for upgrades
ClickGuild=0
; Check "guild" chat's linked items for upgrades
ClickRaid=0
; Check "raid" chat's linked items for upgrades
ClickAny=1
; Check any chat's linked items for upgrades

; The following section(s) set the importance of stats by class shortnames. You must provide these weights, there are no defaults.
[DRU]
AC=2.00
; Set the importance of AC
HP=2.00
; hitpoints
HPReg=0.00
; hitpoint regen
Mana=2.00
ManaReg=0.00
; mana regen
hSTR=0.00
; heroic strength
hSTA=10.00
hAGI=0.00
hDEX=0.00
hINT=0.00
hWIS=15.00
hCHR=0.00
Heal=40.00
; heal bonus
Nuke=40.00
; spell damage bonus
Clrv=3.00
; clairvoyance
Attack=0.00
Haste=0.00
Ratio=0.00
; Weapon ratio
```

If present, this plugin will also load items from `MQ2ItemScoreTemplates.txt` which can evaluate 'quest' gear like "Amorphous Selrach's Boots" for a given class/slot. This takes some work to setup and if you are extremely interested, so are we! Please edit this doc and show us how.

Example configurations for most classes can be found in the [support thread](https://www.redguides.com/community/threads/mq2itemscore.79139/).

## Quick Start

### 1. Load the plugin
```bash
/plugin itemscore load
```

### 2. Configure your stat weights
Set importance values for stats relevant to your class:
```bash
/iscore AC 10
/iscore HP 1
/iscore hSTA 15
```
*(Adjust the values based on your class/lvl, check [/iscore](cmd-iscore.md) for more options)*

### 3. Enable item checking
```bash
/iscore click any
```

### 4. Save your configuration
```bash
/iscore save
```

### 5. Start using
- **Link items** in chat to see if they're upgrades
- **Hold an item** on your cursor and type:
  ```bash
  /iscore
  ```

This will score items against your configured stat weights and report upgrades. 
