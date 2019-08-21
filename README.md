# wcp-ups

## status
 - Now built with `uboonecode v08_00_00_16 -q e17:prof` dependency tree on SL6 and SL7
 - The [setup.sh](https://github.com/HaiwangYu/wcp-ups/blob/master/setup.sh) provided setup example.
 - currently the `paal` and `pid` folders are not included waiting for `glpk` package
 
## usage

```bash
git clone https://github.com/HaiwangYu/wcp-ups.git
```

```bash
source <path-to-wcp-ups>/setup.sh
```

test by
```bash
ups active | grep wcp
```
