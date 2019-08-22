# wcp-ups

## status
 - Available for SL6 and SL7 (ups flavor)
 - refer ups denpendencies in the [wcp.table file](https://github.com/HaiwangYu/wcp-ups/blob/master/wcp/v00_10_00/ups/wcp.table#L21-L23)
  - These dependencies are same in the `uboonecode v08_00_00_16 -q e17:prof` dependency tree
 - The [setup.sh](https://github.com/HaiwangYu/wcp-ups/blob/master/setup.sh) provided setup example.
 - currently the `paal` and `pid` folders are not included waiting for `glpk` package
 
## usage

```bash
git clone https://github.com/HaiwangYu/wcp-ups.git
```

```bash
source <path-to-wcp-ups>/setup.sh
```

## test
```bash
ups active | grep wcp
```
