class ExampleClass1 extends Actor;


const constNoObf1 = "constNoObf1";
const constNoObf2 = "constNoObf2";

const constFromIni1 = "varFromIni1";
const constFromIni2 = "varFromIni2";
const constFakeFromIni = "varFakeFromIni";

const zzConstPrefix1 = "zzVarPrefix1";
const zzConstPrefix2 = "zzVarPrefix2";
const zConstFakePrefix = "zVarFakePrefix";

const constSuffix1_ = "varSuffix1_";
const constSuffix2_ = "varSuffix2_";
const constFakeSuffix_x = "varFakeSuffix_x";

var String varNoObf1;
var String varNoObf2;

var String varFromIni1;
var String varFromIni2;
var String varFakeFromIni;

var String zzVarPrefix1;
var String zzVarPrefix2;
var String zVarFakePrefix;

var String varSuffix1_;
var String varSuffix2_;
var String varFakeSuffix_x;


event spawned() {
	super.spawned();
	
	log("varFromIni1:"@varFromIni1);
	log("varFromIni1:"@varFromIni2);
	log("varFakeFromIni1:"@varFakeFromIni);
	
	log("zzVarPrefix1:"@zzVarPrefix1);
	log("zzVarPrefix1:"@zzVarPrefix2);
	log("zVarFakePrefix1:"@zVarFakePrefix);
	
	log("varSuffix1_:"@varSuffix1_);
	log("varSuffix1_:"@varSuffix2_);
	log("varFakeSuffix1_x:"@varFakeSuffix_x);
}

function funcNoObf1(String s) { log("funcNoObf1"); }
function funcNoObf2(String s) { log("funcNoObf2"); }

function funcFromIni1(String s) { log("funcFromIni1"); }
function funcFromIni2(String s) { log("funcFromIni2"); }
function funcFakeFromIni(String s) { log("funcFakeFromIni"); }

function zzFuncPrefix1(String s) { log("zzFuncPrefix1"); }
function zzFuncPrefix2(String s) { log("zzFuncPrefix2"); }
function zFuncFakePrefix(String s) { log("zFuncFakePrefix"); }

function funcSuffix1_(String s) { log("funcSuffix1_"); }
function funcSuffix2_(String s) { log("funcSuffix2_"); }
function funcFakeSuffix_x(String s) { log("funcFakeSuffix_x"); }


defaultproperties {
	varNoObf1="varNoObf1"
	varNoObf2="varNoObf2"
	varFromIni1="varFromIni1"
	varFromIni2="varFromIni2"
	varFakeFromIni="varFakeFromIni"
	zzVarPrefix1="zzVarPrefix1"
	zzVarPrefix2="zzVarPrefix2"
	zVarFakePrefix="zVarFakePrefix"
	varSuffix1_="varSuffix1_"
	varSuffix2_="varSuffix2_"
	varFakeSuffix_x="varFakeSuffix_x"
}