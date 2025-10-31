using OfficeOpenXml;
using System.ComponentModel;
using System.Text;

namespace proxy_stub
{
    class Program
    {
        private static void ProcessSheet(ExcelWorksheet sheet, List<PacketInfo> packets)
        {
            if (sheet == null) return;

            PacketInfo? current = null;
            int lastRow = sheet.Dimension?.End?.Row ?? 0;
            if (lastRow <= 1) return;

            for (int i = 2; i <= lastRow; i++)
            {
                var caseCell = sheet.Cells[i, 1].Text.Trim();
                var type = sheet.Cells[i, 2].Text.Trim();
                var name = sheet.Cells[i, 3].Text.Trim();

                if (!string.IsNullOrEmpty(caseCell))
                {
                    current = new PacketInfo { Case = caseCell };
                    packets.Add(current);
                }

                if (current != null && !string.IsNullOrEmpty(type) && !string.IsNullOrEmpty(name))
                {
                    current.Fields.Add(new PacketField { Type = type, Name = name });
                }
            }
        }

        static void Main()
        {
            var file = new FileInfo("Packet_Config.xlsx");
            if (!file.Exists)
            {
                Console.WriteLine("파일이 없음");
                return;
            }

            using var package = new ExcelPackage(file);

            var clientRequestSheet = package.Workbook.Worksheets[0];
            var clientPackets = new List<PacketInfo>();
            ProcessSheet(clientRequestSheet, clientPackets);

            var serverResponseSheet = package.Workbook.Worksheets.Count > 1 ? package.Workbook.Worksheets[1] : null;
            var serverPackets = new List<PacketInfo>();
            ProcessSheet(serverResponseSheet, serverPackets);

            var gen = new CodeGenerator(clientPackets, serverPackets);

            File.WriteAllText("PacketHandler_Auto.h", gen.GenerateHandlerHeader());
            File.WriteAllText("PacketProcess_Auto.h", gen.GenerateProcessHeader());
            File.WriteAllText("PacketProcess_Auto.cpp", gen.GenerateProcessCpp());

            File.WriteAllText("PacketMaker_Auto.h", gen.GenerateMakerHeader());
            File.WriteAllText("PacketMaker_Auto.cpp", gen.GenerateMakerCpp());

            Console.WriteLine("Packet *_Auto 파일 생성 완료");
        }
    }
}