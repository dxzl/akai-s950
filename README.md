# akai-s900
S900rs.exe converts and sends 16-bit WAV audio files to your Akai S900/S950 digital sampler. It can also save samples from your machine to a custom file-format. And it can save/restore all of your S900/S950 programs (patches). Works with all Windows versions (as far as I know). You can use a USB to RS232C serial "straight through" adaptor cable with a FTDI chipset. The cable should have a DB25 male connector. In addition to wires for RX/TX, the cable must have wires for CTS/RTS hardware flow-control. It may also be handy to buy a DB25 female to DB25 male null-modem adaptor (that's always been a source of confusion!).

If you just want the Windows exe file click here: https://github.com/dxzl/akai-s950/releases

The project compiles with Embarcadero RAD Studio 10.2 (Tokyo) or higher.

Free compiler: [Embarcadero C++ Builder](https://www.embarcadero.com/products/cbuilder/starter/promotional-download)

Before loading the project you must install the Apro Communication Components using the getit package manager. I use the TApdComPort component

Alternatively, you can manually build/install Apro components to RAD Studio:

https://github.com/TurboPack/AsyncPro

To install AsyncPro on Windows 10:

  1. Unzip the release files into Users\(you)\Documents\Embarcadero\Studio\AsyncPro-master.

  2. Start RAD Studio C++ Builder.

  3. Add the source subdirectory (Users\(you)\Documents\Embarcadero\Studio\AsyncPro-master\source) to the
     IDE's pascal and C++ library paths.
     
  4. Add the hpp subdirectory (C:\Users\(you)\Documents\Embarcadero\Studio\AsyncPro-master\source\hpp\Win32\Release) to the IDE's system include path.     
     
  5. Add \$(BDSLIB)\\$(Platform)\release to the system library path for both pascal and C++.
  
  7. Open & compile the runtime package: Users\(you)\Documents\Embarcadero\Studio\AsyncPro-master\packages\CBuilder\AsyncProCR.cbproj
     
  8. Open & compile the designtime package: Users\(you)\Documents\Embarcadero\Studio\AsyncPro-master\packages\CBuilder\AsyncProCD.cbproj

  9. Right-click AsyncProCD240.bpl and choose install. The IDE should notify you the components have been installed.

To build S900rs.exe, open S950rs.cbproj in RAD Studio C++ Builder, File->Open Project:
  1. Go to Project->Options->Directories & Conditionals and add ..\..\AsyncPro-master\source\hpp\Win32\Release to the to the include path.

  2. Click Project->Build s950rs
  
Contact: dxzl@live.com
