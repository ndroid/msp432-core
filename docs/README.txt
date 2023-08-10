Documentation for MSP432-core libraries

For each libarary, put documentation in docs subdirectory for library, and add 
library doc reference to index page.


For generating docs, place Doxyfile in library root directory and set 

OUTPUT_DIRECTORY       = ../../../docs/LIBRARY_DIR/

Then run doxygen within library root directory. 
