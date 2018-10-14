# B meson cross sections and fragmentation fractions

**Cross sections:**

```channel```: it currently works for the B+ (1), Bd (2) and Bs (4) channels. It can be extended without too much trouble.

```bins```: 'pt', 'y' or 'full'. Double differential, 'pt_y'/'y_pt' is supported but was not tested.

```eff```: boolean

```sys```: boolean. The systematics include both bin dependent and bin-independent sources. The bin independent sources are not calculated automatically, but rather added manually to the ```interface/syst.h``` file. See Analysis Note for understanding all considered systematics.

Example:

```bash
plot_x_sec --channel 1 --bins pt --eff 1 --syst 0
```

**Fragmentation fractions:**

```ratio```: fsfu, fsfd or fdfu

```poly```: boolean. If '1', adds polynomial fit.

```precise_BF_only```: boolean. If '1', it includes only the branching fractions which are known with a small uncertainty. This procedure was followed by ATLAS in their fs/fd paper.

Example:

```bash
plot_ratio --ratio fsfu --bins pt --eff 1 --syst 0 --poly 1 --precise_BF_only 0
```