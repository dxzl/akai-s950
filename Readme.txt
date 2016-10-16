Authored 2016 Scott Swift - This program is distributed under the
terms of the GNU General Public License.

Use Borland's C++ Builder 4 to compile. The free compiler can probably
compile it but I've never tried http://edn.embarcadero.com/article/21205

You also need to download "Apro Freeware Components",
I use the TApdComPort component... not part of
my code...

http://sourceforge.net/projects/tpapro/

http://tpapro.sourceforge.net/ApdComPort.html

NOTE: I copied AsyncPro into my $(BCB)\Projects folder! (Windows 10
has some access issues with writing to the Program Files (x86) directory...)

My C++ Builder Tools->Environment Options Library Path:
$(BCB)\Lib;
$(BCB)\Bin;
$(BCB)\Imports;
$(BCB)\Projects\Lib;
$(BCB)\PROJECTS\Bpl;

My S900 Project Include path:
..\AsyncPro\source;S900;$(BCB)\include;$(BCB)\include\vcl

My S900 Project Library path:
..\AsyncPro\source;S900;$(BCB)\Projects\Lib;$(BCB)\lib\obj;$(BCB)\lib

(NOTE: If you get a compiler error when building Async Pro saying vclmid40.lib is missing,
open the two project files for the component in a text-editor and remove the references
to vclmid40.lib).

Lots of Info. on the S900 Protocol can be found at http://yahcolorize.com/S900/index.htm

- Scott Swift (dxzl@live.com)
