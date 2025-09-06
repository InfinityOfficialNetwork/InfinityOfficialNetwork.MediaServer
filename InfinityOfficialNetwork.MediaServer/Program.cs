using InfinityOfficialNetwork.MediaServer.Native.Transcoding;

namespace InfinityOfficialNetwork.MediaServer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.Out.WriteLine("Hello World");

            NativeLibrary.SetLoggingLevel(AvLogLevel.AvLogInfo);

            NativeLibrary.SetLogging((s, l) =>
            {
                Console.WriteLine($"[{l}] {s}");
            });
            NativeLibrary.Test();
        }
    }
}
