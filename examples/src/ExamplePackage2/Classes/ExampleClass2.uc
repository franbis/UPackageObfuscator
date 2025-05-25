class ExampleClass2 extends ExampleClass1;


var String obf3_;

/*
function funcNoObf1(String msg) {
	super.funcNoObf1("MSG:"@msg);
}
*/

event postBeginPlay() {
	local String obf3_;

	funcNoObf1("FOO1"@obf3_);
}

defaultproperties {
	obf3_="ttt";
}