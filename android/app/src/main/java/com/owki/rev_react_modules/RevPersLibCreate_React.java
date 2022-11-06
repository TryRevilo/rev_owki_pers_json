package com.owki.rev_react_modules;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;

import rev.ca.rev_gen_lib_pers.c_libs_core.RevPersLibCreate;

public class RevPersLibCreate_React extends ReactContextBaseJavaModule {
    public RevPersLibCreate_React(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @NonNull
    @Override
    public String getName() {
        return "RevPersLibCreate_React";
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public void revPersInitReact(String dbPath) {
        new RevPersLibCreate().initRevDb(dbPath);
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public Integer revPersInitJSON(String revEntityJSON) {
        int revRet = (int) new RevPersLibCreate().revPersInitJSON(revEntityJSON);

        return revRet;
    }
}
