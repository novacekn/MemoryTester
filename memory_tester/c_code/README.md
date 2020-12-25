# Memtester

This is a custom version of the memory tester used on the Chromebooks called [memtester](https://github.com/jnavila/memtester).
It is heavily modified to meet the needs I needed it for which was to have it output a progress that I could use in a PyQt5
progress bar and to have the ability to run each test individually. There are 16 different memory tests:

* Stuck Address (stuck_address)
* Random Value (random_value)
* XOR Comparison (xor_comparison)
* SUB Comparison (sub_comparison)
* MUL Comparison (mul_comparison)
* DIV Comparison (div_comparison)
* OR Comparison (or_comparison)
* AND Comparison (and_comparison)
* Sequential Increment Comparison (sequential_increment_comparison)
* Solid Bits Comparison (solid_bits_comparison)
* Checkerboard Comparison (checkerboard_comparison)
* Block Sequential Comparison (block_sequential_comparison)
* Walking Zeroes Comparison (walking_zeroes_comparison)
* Walking Ones Comparison (walking_ones_comparison)
* Bit Spread Comparison (bit_spread_comparison)
* Bit Flip Comparison (bit_flip_comparison)

### Building
A make file is included with two options (mac or linux). To build for mac run ```make mac``` and to build
for linux run ```make linux```. Note that the only difference between the two commands is the name of the output file.

### Running
To run the program run ```./memtester_<mac|linux> <bytes> <test>```. For example, if we were running this on linux
and wanted to test 100 MB of memory with walking ones, we would run ```./memtester_linux 1000000 walking_ones_comparison```.
