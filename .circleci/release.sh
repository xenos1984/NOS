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
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @i686-pc-elf/NOS.iso "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-i686.iso&label=NOS%20bootable%20ISO%20%2Ei686%2F"

# i686 floppy image
#curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @i686-pc-elf/NOS.img.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-i686.img.bz2&label=NOS%20bootable%20floppy%20%2Ei686%2F"

# x86_64 CD image
curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @x86_64-pc-elf/NOS.iso "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-x86_64.iso&label=NOS%20bootable%20ISO%20%2Ex86_64%2F"

# x86_64 floppy image
#curl -X POST -u xenos1984:$GHTOKEN -H 'Content-Type: application/octet-stream' --data-binary @x86_64-pc-elf/NOS.img.bz2 "https://uploads.github.com/repos/xenos1984/NOS/releases/9063364/assets?name=nos-x86_64.img.bz2&label=NOS%20bootable%20floppy%20%2Ex86_64%2F"

