using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using ExampleApp;
using HarmonyLib;

namespace ExampleMod;

public class Mod
{
    [UnmanagedCallersOnly]
    public static void Patch()
    {
        Console.WriteLine(
            "Load context: " + AssemblyLoadContext.GetLoadContext(typeof(Program).Assembly)
        );
        Harmony.DEBUG = true;
        Harmony harmony = new("com.example.patch");
        MethodInfo target = AccessTools.Method("ExampleApp.Program:checkSubscription"); // this is required if Patch() is being called from a different ALC than the Default one
        var postfix = new HarmonyMethod(
            typeof(ProgramPatches).GetMethod(
                "Postfix",
                BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic
            )
        );
        harmony.Patch(target, postfix: postfix);
        var previous = Console.BackgroundColor;
        Console.BackgroundColor = ConsoleColor.DarkRed;
        Console.WriteLine("PATCHING");
        Console.BackgroundColor = previous;
    }
    public class ProgramPatches
    {
        static void Postfix(ref bool __result)
        {
            Console.WriteLine("hacking...");
            __result = true;
        }
    }
}
