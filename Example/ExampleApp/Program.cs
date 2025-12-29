using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Runtime.Loader;

namespace ExampleApp;

public class Program
{
    public static string status = "free";

    [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
    public static bool checkSubscription()
    {
        Console.WriteLine("Checking subscription status...");
        return status == "pro";
    }

    [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
    public static void Main()
    {
        while (true)
        {
            Console.WriteLine(status);
            Console.WriteLine(checkSubscription());
            Thread.Sleep(4000);
        }
    }
}
