/*
 * Copyright (c) 2014-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== Cache.xs ========
 */

var Cache;
var Reset;

var origSizes = {
    "l1PMode" : null,
    "l1DMode" : null,
    "l2Mode" : null,
};

/*
 * ======== getAsmFiles ========
 * return the array of assembly language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getAsmFiles(targetName)
{
    switch(targetName) {
        case "ti.targets.elf.nda.C71":
            return (["Cache_asm.s71"]);
            break;

        default:
            return (null);
            break;
    }
}

/*
 *  ======== getCacheSizeString ========
 *  This function converts the cache size to the string specified in the
 *  device *.xdc file used as a hash table.
 */
function getCacheSizeString(value)
{
    switch (value) {
        case Cache.L1Size_0K:
        case Cache.L2Size_0K:
            return ("0k");
            break;
        case Cache.L1Size_4K:
            return ("4k");
            break;
        case Cache.L1Size_8K:
            return ("8k");
            break;
        case Cache.L1Size_16K:
            return ("16k");
            break;
        case Cache.L1Size_32K:
        case Cache.L2Size_32K:
            return ("32k");
            break;
        case Cache.L2Size_64K:
            return ("64k");
            break;
        case Cache.L2Size_128K:
            return ("128k");
            break;
        case Cache.L2Size_256K:
            return ("256k");
            break;
        case Cache.L2Size_512K:
            return ("512k");
            break;
        case Cache.L2Size_1024K:
            return ("1024k");
            break;
    }
    return (undefined);
}

/*
 *  ======== getL1CacheSize ========
 *  This function converts the L1 cache string to a cache size enumeration.
 */
function getL1CacheSize(size)
{
    switch (size) {
        case "0k":
            return (Cache.L1Size_0K);
            break;
        case "4k":
            return (Cache.L1Size_4K);
            break;
        case "8k":
            return (Cache.L1Size_8K);
            break;
        case "16k":
            return (Cache.L1Size_16K);
            break;
        case "32k":
            return (Cache.L1Size_32K);
            break;
    }
    return (undefined);
}

/*
 *  ======== getL2CacheSize ========
 *  This function converts the L2 cache string to a cache size enumeration.
 */
function getL2CacheSize(size)
{
    switch (size) {
        case "0k":
            return (Cache.L2Size_0K);
            break;
        case "32k":
            return (Cache.L2Size_32K);
            break;
        case "64k":
            return (Cache.L2Size_64K);
            break;
        case "128k":
            return (Cache.L2Size_128K);
            break;
        case "256k":
            return (Cache.L2Size_256K);
            break;
        case "512k":
            return (Cache.L2Size_512K);
            break;
        case "1024k":
            return (Cache.L2Size_1024K);
            break;
    }
    return (undefined);
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    Cache = this;

    /* provide getAsmFiles() for Build.getAsmFiles() */
    this.$private.getAsmFiles = getAsmFiles;

    /* set the original sizes to what's defined in Cache.xdc */
    origSizes["l1pSize"] = Cache.initSize.l1pSize;
    origSizes["l1dSize"] = Cache.initSize.l1dSize;
    origSizes["l2Size"] = Cache.initSize.l2Size;

    /* set fxntab default */
    Cache.common$.fxntab = false;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    xdc.useModule("ti.sysbios.hal.Hwi");
    Reset = xdc.useModule("xdc.runtime.Reset");

    Reset.fxns[Reset.fxns.length++] = Cache.startup;

    /*
     *  Override the original sizes if we find the cache settings
     *  in the platform.
     */
    if ("l1PMode" in Program.platform.$orig ||
        "l1DMode" in Program.platform.$orig ||
        "l2Mode" in Program.platform.$orig) {
        if (Program.platform.$orig.l1PMode != null) {
            origSizes["l1pSize"] =
                getL1CacheSize(Program.platform.$orig.l1PMode);
        }
        if (Program.platform.$orig.l1DMode != null) {
            origSizes["l1dSize"] =
                getL1CacheSize(Program.platform.$orig.l1DMode);
        }
        if (Program.platform.$orig.l2Mode != null) {
            origSizes["l2Size"] = getL2CacheSize(Program.platform.$orig.l2Mode);
        }
    }

    /* Check if Cache.initSize written by cfg script */
    if (Cache.initSize.$written("l1pSize") ||
        Cache.initSize.$written("l1dSize") ||
        Cache.initSize.$written("l2Size")) {
        if ((Cache.initSize.l1pSize != origSizes["l1pSize"]) ||
            (Cache.initSize.l1dSize != origSizes["l1dSize"]) ||
            (Cache.initSize.l2Size != origSizes["l2Size"])) {
            Cache.$logWarning("Cache settings were changed in user " +
                "configuration. User configuration options will override " +
                "platform settings. Check your memory map to make sure " +
                "that Cache does not conflict with your L1/L2 memory " +
                "placement. To avoid conflicts between L1/L2 memory and " +
                "cache, we recommended specifying cache sizes along with " +
                "memory sizes in a platform package.", this, null);
        }
    }
    else {
        /* if cfg script never wrote Cache.initSize, set them to origSizes */
        Cache.initSize.l1pSize = origSizes["l1pSize"];
        Cache.initSize.l1dSize = origSizes["l1dSize"];
        Cache.initSize.l2Size  = origSizes["l2Size"];
    }
}

/*
 *  ======== getL1CacheSizeString ========
 *  This function converts the cache size to the string specified in the
 *  device *.xdc file used as a hash table.
 */
function getL1CacheSizeString(value)
{
    switch (value) {
        case 0:
            return ("0k");
            break;
        case 1:
            return ("4k");
            break;
        case 2:
            return ("8k");
            break;
        case 3:
            return ("16k");
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            return ("32k");
            break;
        default:
            return ("Unknown");
    }
}

/*
 *  ======== getL2CacheSizeString ========
 *  This function converts the cache size to the string specified in the
 *  device *.xdc file used as a hash table.
 */
function getL2CacheSizeString(value)
{
    switch (value) {
        case 0:
            return ("0k");
            break;
        case 1:
            return ("32k");
            break;
        case 2:
            return ("64k");
            break;
        case 3:
            return ("128k");
            break;
        case 4:
            return ("256k");
            break;
        case 5:
            return ("512k");
            break;
        case 6:
            return ("1024k");
            break;
        case 7:
            return ("2048k");
            break;
        default:
            return ("Unknown");
    }
}

/*
 *  ======== convertToUInt32 ========
 */
function convertToUInt32(value)
{
    /*
     *  Provide this workaround here due to javascript.
     *  Note: Javascript converts numbers to 32-bit signed numbers before
     *        evaluating bitwise operators.
     */
    if (value < 0) {
        value = value + 0xffffffff + 1;
    }

    return (value);
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Task module view.
 */
function viewInitModule(view, mod)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    var cacheModCfg = Program.getModuleConfig('ti.sysbios.family.c7x.Cache');

    /* Retrieve the L1P cache size */
    try {
        var L1PCacheSize = Program.fetchStruct(ScalarStructs.S_UInt32$fetchDesc,
                                               cacheModCfg.L1PCFG,
                                               false);
    }
    catch (e) {
        print("Caught exception trying to retrieve L1P cache size: " + e);
    }

    /* Retrieve the L1D cache size */
    try {
        var L1DCacheSize = Program.fetchStruct(ScalarStructs.S_UInt32$fetchDesc,
                                               cacheModCfg.L1DCFG,
                                               false);
    }
    catch (e) {
        print("Caught exception trying to retrieve L1D cache size: " + e);
    }

    /* Retrieve the L1P cache mode */
    try {
        var L1PCacheMode = Program.fetchStruct(ScalarStructs.S_UInt32$fetchDesc,
                                               cacheModCfg.L1PCC,
                                               false);
    }
    catch (e) {
        print("Caught exception trying to retrieve L1P cache mode: " + e);
    }

    /* Retrieve the L1D cache mode */
    try {
        var L1DCacheMode = Program.fetchStruct(ScalarStructs.S_UInt32$fetchDesc,
                                               cacheModCfg.L1DCC,
                                               false);
    }
    catch (e) {
        print("Caught exception trying to retrieve L1D cache mode: " + e);
    }

    /* Retrieve the L2 cache configuration */
    try {
        var L2Cache = Program.fetchStruct(ScalarStructs.S_UInt32$fetchDesc,
                                          cacheModCfg.L2CFG,
                                          false);
    }
    catch (e) {
        print("Caught exception trying to retrieve L2 cache size: " + e);
    }

    /* The 3 LSB determine the cache configuration size */
    var L1PSize = L1PCacheSize.elem & 0x7;
    var L1DSize = L1DCacheSize.elem & 0x7;
    var L2Size = L2Cache.elem & 0x7;

    view.L1PCacheSize = getL1CacheSizeString(L1PSize);
    view.L1DCacheSize = getL1CacheSizeString(L1DSize);
    view.L2CacheSize = getL2CacheSizeString(L2Size);

    /* Determine the cache configuration mode */
    view.L1PMode = (L1PCacheMode.elem & 7) ? "Freeze" : "Normal";
    view.L1DMode = (L1DCacheMode.elem & 7) ? "Freeze" : "Normal";

    var L2Mode = L2Cache.elem & 0x18;
    switch (L2Mode) {
        case 0x0:
            view.L2Mode = "Normal";
            break;
        case 0x8:
            view.L2Mode = "Freeze";
            break;
        case 0x10:
        case 0x18:
            view.L2Mode = "Bypass";
            break;
        default:
            view.L2Mode = "Unknown";
    }
}

/*
 *  ======== viewInitSystemInts ========
 */
function viewInitMarRegisters(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    var Cache = xdc.useModule('ti.sysbios.family.c7x.Cache');
    var numMarRegisters = 256;

    /* Retrieve the module configuration. */
    var modCfg = Program.getModuleConfig('ti.sysbios.family.c7x.Cache');

    /* Retrieve the MAR registers */
    try {
        var marRegisters = Program.fetchArray(ScalarStructs.S_UInt32$fetchDesc,
                                              modCfg.MAR,
                                              numMarRegisters,
                                              false);
    }
    catch (e) {
        // TODO - Report problem
        print("Caught exception while trying to retrieve MAR registers: " + e);
    }

    /*
     *  Display each MAR register that is enabled.
     *  MAR registers 0-15 are read-only and need not be displayed.
     */
    for (var i = 16; i < numMarRegisters; i++) {
        var marview =
            Program.newViewStruct('ti.sysbios.family.c7x.Cache',
                                  'MARs');

        marview.number = i;
        marview.addr   = utils.toHex(marRegisters[i].$addr);
        marview.startAddrRange =
            utils.toHex(convertToUInt32(Number(i << 24)));
        marview.endAddrRange   =
            utils.toHex(convertToUInt32(Number((i << 24) + 0xFFFFFF)));
        marview.cacheable = marRegisters[i].elem & modCfg.PC;
        marview.prefetchable = marRegisters[i].elem & modCfg.PFX;
        marview.marRegisterValue = "0x" + marRegisters[i].elem.toString(16);

        view.elements.$add(marview);
    }
}
