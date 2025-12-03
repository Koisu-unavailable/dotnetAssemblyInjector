using System.Runtime.InteropServices;
using ExampleApp;
using HarmonyLib;

namespace ExampleMod;

public class Mod
{
    [UnmanagedCallersOnly]
    static void Patch()
    {
        new Harmony("coolPatch").PatchAll();
    }
}

[HarmonyPatch(typeof(Program))]
[HarmonyPatch(nameof(Program.checkSubscription))] 
class Patch01
{
    static bool Prefix(Program __instance, ref string ___status)
    {
        ___status = "pro";
        return true;
    }
}
