cd ./3rdparty
tar -xvf ./podofo-0.9.3.tar.gz
cd podofo-0.9.3/
mkdir ../podofo-build
cd ../podofo-build/
cmake -G "Unix Makefiles" ../podofo-0.9.3
make
sudo make install
