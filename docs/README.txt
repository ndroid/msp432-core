Documentation for MSP432-core libraries

For each libarary, put documentation in docs subdirectory for library, and add 
library doc reference to index page.


For generating docs, place Doxyfile in library root directory and set 

    OUTPUT_DIRECTORY       = ../../../docs/LIBRARY_DIR/

    Then run doxygen within library root directory. 


Info for referencing root main page from library main pages

    Add a custom Doxygen Layout file.

    Run doxygen using doxygen -l to generate the default layout file DoxygenLayout.xml. 
    Then add it to your Doxyfile

    LAYOUT_FILE = DoxygenLayout.xml

    To change the top bar you want to change entries under <navindex>

    More info on this: http://www.doxygen.nl/manual/customize.html#layout

    Modify the beginning of <navindex> to

    <tab type="user" url="../index.html" title="Main Page"/>
    <tab type="mainpage" visible="yes" title="<LIBRARY NAME>"/>
