using InfinityOfficialNetwork.MediaServer.Native.Interop;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Logging.Console;

namespace InfinityOfficialNetwork.MediaServer
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            Console.Out.WriteLine("Hello World");

            LoggingConfiguration configuration = new LoggingConfiguration {
                ClassLevelOverrides = new Dictionary<string,AvLogLevel>(),
                Level = AvLogLevel.AvLogVerbose,
                Logger = (level, message, cls) =>
                {
                    Console.WriteLine($"[{level}] [{cls}] {message}");
                }
            };

            NativeLibraryConfig.ConfigureLogging(configuration);

            MediaContainer mediaContainer = new MediaContainer();

            mediaContainer.Data = await File.ReadAllBytesAsync(@"C:\Users\InfiniPLEX\Videos\den pobedy.mp4");

            var metadata = MetadataReader.GetMetadata(mediaContainer);

            foreach (var metadataItem in metadata.Metadata)
            {
                Console.WriteLine($"{metadataItem.Key}: {metadataItem.Value}");
            }
        }
    }
}
