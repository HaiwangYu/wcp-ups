# wcp-ups

## status
 - Available for SL6 and SL7 (ups flavor)
 - refer ups denpendencies in the wcp.table file
   - These dependencies are consistent with the `uboonecode v08_00_00_* -q e17:prof` dependency tree
   - I did not check which other uboonecode versions also used these dependencies
 - The [setup.sh](https://github.com/HaiwangYu/wcp-ups/blob/master/setup.sh) provided setup example.
 - currently the `paal` and `pid` folders are not included waiting for `glpk` package
 
## usage

Clone repository
```bash
git clone https://github.com/HaiwangYu/wcp-ups.git
```

Untar a tarball version
```bash
tar -xf wcp.v00_11_00.tar
```

Source setup.sh
```bash
source <path-to-wcp-ups>/setup.sh
```

## test
```bash
ups active | grep wcp
```
