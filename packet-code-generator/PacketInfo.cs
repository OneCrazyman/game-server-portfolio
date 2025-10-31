
namespace proxy_stub
{
    public class PacketField
    {
        public string Type { get; set; }
        public string Name { get; set; }
    }

    public class PacketInfo
    {
        public string Case { get; set; }
        public List<PacketField> Fields { get; set; } = new();
    }
}