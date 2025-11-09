namespace ExampleApp;

public class Program
{
    public static bool checkSubscription(string status)
    {
        return status == "pro" ? true : false;
    }
    public static void Main()
    {
        while (true)
        {
            Console.WriteLine(checkSubscription("free"));
            Thread.Sleep(1);
        }
    }
}