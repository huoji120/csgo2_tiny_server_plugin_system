### about this project
name: "cs2 server plugin system"  
author: "huoji"  
description: "a server plugin system for cs2 server,power by lua"  
note:  
```
This is only experimental, with the aim of filling in the gap before the release of source2mod. Please do not treat it as a long-term plugin, and I will not provide support 
```

### credit  
https://github.com/alliedmodders/hl2sdk/  
https://github.com/Source2ZE/CS2Fixes/  
https://github.com/bruhmoment21/cs2-sdk/  
https://github.com/neverlosecc/source2sdk  

### feature list 
1. lua plugin system 
2. metamod compatible 
3. vscript patch, support vscript plugin and vscript compatible 
4. http/https request method 

### warning
```
The loader has injection behavior, and I cannot guarantee whether it will be marked by VAC (I am not sure if the server has VAC protection). I am not responsible for this

also, the loader will be recognized as a malicious program by antivirus software because injection behavior
```

### Usage 
1. use vs2019 compile csgo2.dll and loader.exe 
2. create a config.json in loader.exe directory: 
```
{
  "servers": [
    {
        "dll": "your path in csgo2.dll in vs2019 complie",
        "path": "path for cs2.exe",
        "command": "cs2.exe server run command"
    }
  ]
}
```
example:  
```
{
  "servers": [
    {
      "dll": "C:\\Users\\Administrator\\Desktop\\loader\\csgo2.dll",
      "path": "C:\\Program Files (x86)\\steam\\steamapps\\common\\Counter-Strike Global Offensive\\game\\bin\\win64\\cs2.exe",
      "command": "-ip 0.0.0.0 -dedicated -game csgo -console -usercon +game_type 0 +game_mode 0 +mp_maxrounds 30 -maxplayers_override 64 +exec server.cfg +exec gamemode_retakecasual.cfg +map de_dust2 +mp_endmatch_votenextleveltime 20 +mp_endmatch_votenextmap 1 +hostname xxxxxx"
    }
  ]
}

```
3. run loader.exe as administrator, cs2 server will start
4. create `huoji_scripts` in cs2.exe dirctory
5. create plugin dirctory name ,like `funny_trash` is my plugin name
6. put your own plugin script in you plugin dircory name 
### Test Plugin  
api and plugin example: 
https://github.com/huoji120/cs2_funny_trash_plugin  
