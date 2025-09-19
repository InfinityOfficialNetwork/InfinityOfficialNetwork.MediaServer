using InfinityOfficialNetwork.MediaServer.Native.Interop;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Logging.Console;

namespace InfinityOfficialNetwork.MediaServer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.Out.WriteLine("Hello World");

            LoggingConfiguration configuration = new LoggingConfiguration {
                ClassLevelOverrides = new Dictionary<string,AvLogLevel>(),
                Level = AvLogLevel.AvLogVerbose,
                Logger = (level, message, cls, tid) =>
                {
                    Console.WriteLine($"[{level}] [{tid}] [{cls}] {message}");
                }
            };

            NativeLibraryConfig.ConfigureLogging(configuration);

            byte[] file = File.ReadAllBytesAsync(@"C:\Users\InfiniPLEX\Videos\videoplayback.mp4").Result;
            
            MediaContainer input = new ArrayBackedMediaContainer(file);

            MediaContainer output = Transcoder.TranscodeAsync(input).Result;

            //MediaContainer mediaContainer = new MediaContainer();

            //mediaContainer.Data = await File.ReadAllBytesAsync(@"C:\Users\InfiniPLEX\Videos\den pobedy.mp4");

            //var metadata = MetadataReader.GetMetadata(mediaContainer);

            //foreach (var metadataItem in metadata.Metadata)
            //{
            //    Console.WriteLine($"{metadataItem.Key}: {metadataItem.Value}");
            //}
        }
    }
}
