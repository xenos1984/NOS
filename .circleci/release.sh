#!/bin/bash

# Update "latest" tag.
curl -X PATCH -u xenos1984:$GHTOKEN -d '{"sha": "'$CIRCLE_SHA1'"}' https://api.github.com/repos/xenos1984/nos/git/refs/tags/latest

# Delete outdated release assets.
for asset in `curl https://api.github.com/repos/xenos1984/nos/releases/9063364/assets | grep -o 'https.*assets/[0-9]*'`
do
	curl -X DELETE -u xenos1984:$GHTOKEN $asset
done

# Upload freshly built release assets.

# i686 CD image
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @i686-pc-elf/NOS.iso.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-i686.iso.bz2&label=NOS%20bootable%20ISO%20%28i686%29"

# i686 floppy image (FAT12)
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @i686-pc-elf/NOS-fat12.img.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-i686-fat12.img.bz2&label=NOS%20bootable%20floppy%20%28i686-FAT12%29"

# i686 floppy image (EXT2)
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @i686-pc-elf/NOS-ext2.img.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-i686-ext2.img.bz2&label=NOS%20bootable%20floppy%20%28i686-EXT2%29"

# x86_64 CD image
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @x86_64-pc-elf/NOS.iso.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-x86_64.iso.bz2&label=NOS%20bootable%20ISO%20%28x86_64%29"

# x86_64 floppy image (FAT12)
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @x86_64-pc-elf/NOS-fat12.img.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-x86_64-fat12.img.bz2&label=NOS%20bootable%20floppy%20%28x86_64-FAT12%29"

# x86_64 floppy image (EXT2)
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @x86_64-pc-elf/NOS-ext2.img.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-x86_64-ext2.img.bz2&label=NOS%20bootable%20floppy%20%28x86_64-EXT2%29"

