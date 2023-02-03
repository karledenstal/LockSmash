# Brute Force - Lock Overhaul SKSE

This is my first SKSE plugin. Other mods exists like this, but couldn't find one that did it in an SKSE plugin, so used it as a learning experience. Will probably work on this more to polish it in the future.

**What does it do different?**

I love playing as dumb barbarians that wouldn't care about lockpicking, but I also don't like restraining myself to a set level cap in order to access loot. Mostly because Skyrim, obviously, wasn't built that way. So instead of setting a minimum skill level in order to break open locks, I set a requirement for equipment instead.

The idea being that locks of a harder degree would be more sturdy and harder to break open. So while any weapon can bash open a novice lock, you'd need a steel weapon or better to break open an apprentice lock, and so on, until you need your daedric or dragonbone to access the master locks. While this could feel like a hindrance, I think it's a nice way to balance it, since you could rely on crafting, looting or just having luck finding a good weapon to break open locks. And you can still of course just lockpick it.

There are other modpacks that add unlock spells, so I left those out of this one. I might add them in the future, but for now I'm happy with this.

**Features:**

- Break locks using brute force
﻿Novice locks: Steel, Silver
        Apprentice locks: Dwarven, Elven
        Adept locks: Orcish, Nordic
        Expert locks: Glass, Ebony, Stalhrim
        Master locks: Daedric, Dragonbone

**Optional Features:**

There is a BruteForce.ini file that you can configure to adjust the gamplay experience, such as:

- Only allow blunt weapons to open locks
- Only allow two handed weapons to open locks
- Only allow warhammers to open locks
- Enable/disable skill requirement
- Configure skill increase
- Configure required skill to bash open locks
- Enabled/disable the mod.

**Roadmap**

**Version 2.0:**
- MCM config (with help of MCM helper)
- Better detection event
- More immersive calculation of lock bash to not always guarantee success
- "Simple" lock bash by using Activate event instead of Hit.
- Option to disable lockpicking if skill doesn't meet requirement

**Version 3.0**
- Use alchemy to melt locks
- Use shouts to break open locks.
- Destruction magic can unlock chests.
- Autopicking

**Known "issues":**
- Detection/Steal alarm seems to work a bit weird, and gives a really low bounty atm

**Compatibility:**
- Not compatible with mods that do similar things.