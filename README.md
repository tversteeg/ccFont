# ccFont
Font utily C library and a command line interface to convert fonts.

Screenshot:

![ccfconv](http://imgur.com/MdO1tz5.png)

## Building
### Generic
Install [ccore](https://github.com/ccore/ccore), and make.

    $ make

### Linux distro's with dpkg
Install dev-scripts `apt-get install dev-scripts`

    $ debuild -us -uc -i -b

## Install
### Generic

    # make install
    
### Linux distributions with dpkg

    # dpkg -i ../libccfont*.deb
