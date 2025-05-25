# Running the Examples

**NOTE**: &nbsp;These instructions assume you're on Windows.

You can find some example packages at `examples\` setup to test the obfuscator out.
<br>
The examples are setup in a way that only names starting with *`zz`* and/or ending with *`_`* will be obfuscated. Both the packages will get their script text objects content replaced with a custom one from a text file present in the same directory.

<br>


## Obfuscation Instructions

1. Run `setup.bat` from the app directory to complete the setup, if you haven't yet
1. Copy `Core.u`, `Engine.u` to the app directory
1. Run `obfuscate.bat` from `examples\`


## In-Game Usage Instructions

In case you wish to test the obfuscated packages in-game, here are the steps:

1. Place the obfuscated packages into the `System` directory of the game
1. Remove the *`-obfuscated`* suffix from the packages names
3. Start the game and send one or both of these commands:
`summon ExamplePackage1.ExampleClass1`
`summon ExamplePackage2.ExampleClass2`
4. Send the command `showlog` to check that everything worked correctly