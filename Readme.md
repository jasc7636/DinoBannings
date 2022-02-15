# Dino Bannings
A plugin for the [Ark Server Api](https://arkserverapi.com/index.php?resources/ark-server-api.4/).
It can disallow the download and release of certain dinos or prevents them from flying.

It can be configured with a _config.json_ file. Down below is a example configuration.
```
{
  "DisallowRelease": {
    "TheIsland": [],
    "TheCenter": [],
    "ScorchedEarth_P": [],
    "Ragnarok": [],
    "Aberration_P": [
      "GasBag",
      "MilkGlider",
      "IceJumper",
      "LionfishLion",
      "Sinomacrops"
    ],
    "Extinction": [],
    "Valguero_P": [],
    "Genesis": [],
    "CrystalIsles": [],
    "Gen2": [],
    "LostIsland": []
  },
  "PreventFlying": {
    "PlayerMessage": "You Can't Fly Here",
    "TheIsland": [],
    "TheCenter": [],
    "ScorchedEarth_P": [],
    "Ragnarok": [],
    "Aberration_P": [
      "HoverSkiff"
    ],
    "Extinction": [],
    "Valguero_P": [],
    "Genesis": [],
    "CrystalIsles": [],
    "Gen2": [],
    "LostIsland": []
  }
}
```