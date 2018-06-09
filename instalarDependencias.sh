#!/bin/bash
git clone https://github.com/sisoputnfrba/so-commons-library
cd so-commons-library
sudo make install
cd ..
git clone https://github.com/sisoputnfrba/parsi
cd parsi
sudo make install
cd ..