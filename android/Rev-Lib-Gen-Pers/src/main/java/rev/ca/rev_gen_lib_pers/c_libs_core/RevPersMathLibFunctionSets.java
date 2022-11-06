package rev.ca.rev_gen_lib_pers.c_libs_core;

public class RevPersMathLibFunctionSets {

    static {
        System.loadLibrary("hello-libs");
    }

    public native String stringFromJNI();
}
