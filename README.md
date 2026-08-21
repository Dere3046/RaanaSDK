# lkm-template

module template for the Dere3046 LKM ecosystem. deps are declared by
name, revs pinned in deps.lst, the KMSDK package manager resolves
urls and the transitive closure.

## build

```sh
scripts/build-ddkk.sh android16-6.12
```

full targets: android12-5.10 android13-5.10 android13-5.15
android14-5.15 android14-6.1 android15-6.6 android16-6.12

## deps

```sh
scripts/fetch-deps.sh    # clone KMSDK at .sdk-version, deploy deps.lst
.sdk/scripts/sdk ls      # list registry
.sdk/scripts/sdk add HooKern         # add by registry rev
.sdk/scripts/sdk remove HooKern      # drop from deps.lst
.sdk/scripts/sdk update KallRecon    # bump to registry rev
.sdk/scripts/sdk audit               # detect libs used but undeclared
```
