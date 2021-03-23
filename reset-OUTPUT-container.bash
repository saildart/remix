#!/bin/bash
echo BEFORE
        df -T -t ext4 -h /d/large/OUTPUT

        sudo umount /d/large/OUTPUT
        sudo mkfs.ext4 -F /d/large/OUTPUT.container
        sudo mount  /d/large/OUTPUT
        sudo chown bgb:bgb /d/large/OUTPUT
        rmdir /d/large/OUTPUT/lost+found
        
echo AFTER
        df -T -t ext4 -h /d/large/OUTPUT
