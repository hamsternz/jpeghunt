# jpeghunt
Hunt out JPEG (JFIF/EXIF) files in hard disk images and extracts them.

NOTE - THIS IS A HACK!

I had an external 240GB USB drive that would not stay mounted - it was due to some failed sectors. 

It had some photos on it I wanted to keep, so I used DDRESCUE to extract most of the data from the drive into an image file, but it was still not mountable or FSCKable.

I then wrote this to hunt out JPEG images and extract them. Results were Surprisingly good. Got back 40k+ images, with nearly all of them being perfect.

## Usage

Build with "make"

    make

Create an directory called "out":

    mkdir out

The run against whatever your disk image is called:

    ./jpeghunt disk.img


It reports progress along thw way

    Extracted 'out/0000000241.jpg'
    Extracted 'out/0000000242.jpg'
    Extracted 'out/0000000243.jpg'
    
    1 GB inspected
    2000000 sectors inspected
    280 start_of_image count
    242 APP0 count
    93 EXIF count
    35 FFED count
    403 FFD8 count
    0 FFDB count
    Extracted 'out/0000000244.jpg'
    Extracted 'out/0000000245.jpg'


