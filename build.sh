echo "Moving files..."
mv ./Mifare\ Classic/* ./
mv ./Mifare\ Ultralight/* ./
mv ./misc/* ./
echo "Building mifare.c..."
gcc -std=c99 -lnfc -c -o mifare.c.o mifare.c
echo "Building all c++ files..."
find ./*.cpp -exec g++-4.7 -lnfc -std=c++11 -c -o {}.o {} \;
echo "Linking all files..."
g++-4.7 -std=gnu++11 -lnfc -o card_scanner ./mifare.c.o ./*.cpp.o