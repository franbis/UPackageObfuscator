<h1 style="display: flex; align-items: center; flex-wrap: wrap;">
    <img src="res/icons/icon.ico" width="100" alt="logo">
    <div>UPackageObfuscator</div>
</h1>

A standalone Unreal Engine 1 packages obfuscator CLI that runs on Unreal Engine.
<br>
<br>
It can automatically generate a list of names to obfuscate, it can also obfuscate by names prefix and/or suffix, or do a combination of these.
<br>
There's no need to type any bogus name as they get automatically generated either in a completely random manner or based on name hashes, you choose.

## Compatible packages

* Any *Unreal Tournament (1999)* package
* Any other Unreal Engine 1 package as long as it doesn't make use of features not present in built-in *Unreal Tournament* packages (E.g. A *Rune* package with `Core.u` as *only* dependency should be fine)

<br>
<br>

Obfuscated package when disassembled using *ScriptRays* (in *UnrealEd*)
<br>
<img src="Examples/img/scriptrays_preview_2_512.avif" width="256" alt="ScriptRays preview 2" />

Obfuscated package when disassembled using *UE Explorer*
<br>
<img src="Examples/img/ue_explorer_preview_1_512.avif" width="256" alt="UE Explorer preview 1" />

Obfuscated package when disassembled using *UTPT*
<br>
<img src="Examples/img/utpt_preview_1_512.avif" width="256" alt="UTPT preview 1" />


## Can bogus names be deobfuscated?

If you let the app generate completely random bogus names (which is the default behavior), not at all!
<br>
The original names get lost forever.

## How do deobfuscators react?

By default, the script text gets nullified (not just erased), this prevents any software from finding the object reference.
* ScriptRays (469e) won't be accessible from UnrealEd as an error message will inform the package has no script text
* UE Explorer won't show the ScriptText entry
* UTPT will show an empty panel when clicking on "View the Script"

<br>

## Build Instructions

### Requirements
* The Unreal Engine DLLs and their localization files: `Core.dll`, `Engine.dll`, `Core.int`, `Engine.int`

### Windows (Visual Studio)
1. Remove the suffix `.template` from `UnrealEngine.props` and update the internal paths accordingly
1. Use the `Release` configuration and set the platform to `x86`
1. Build the solution
1. There should be no need to run `setup.bat` from the output directory, run it if a startup issue occurs

## Usage Instructions

### Windows
If you wish to run the examples, head to [Running the Examples](/Examples/examples.md).

The app follows the Unreal Engine 1 standards for standalone apps, therefore it needs to be ran just like you would run UCC for example.
<br>
Here are the steps to obfuscate a package using a name list:

1. Run `setup.bat` to complete the setup, if you haven't yet
1. Place the package to be obfuscated in the app directory
1. Ensure all the package dependencies are in the same directory
1. On a command prompt, send this command to extract all the names from the package:
<br>
`UPackageObfuscator INPUT_PACKAGE_NAME exportnames=names.ini`
1. Open `names.ini` and remove any name you don't wish to obfuscate
1. On a command prompt, send this command:
<br>
`UPackageObfuscator INPUT_PACKAGE_NAME names=names.ini out=OUTPUT_PACKAGE_NAME`