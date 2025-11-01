using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace proxy_stub
{
    public class CodeGenerator
    {
        private readonly List<PacketInfo> clientPackets_;
        private readonly List<PacketInfo> serverPackets_;

        public CodeGenerator(List<PacketInfo> clientPackets, List<PacketInfo> serverPackets)
        {
            clientPackets_ = clientPackets;
            serverPackets_ = serverPackets;
        }

        private static string ToMacroStyle(string name)
        {
            var sb = new StringBuilder();
            for (int i = 0; i < name.Length; i++)
            {
                char c = name[i];
                if (char.IsUpper(c) && i > 0)
                    sb.Append('_');
                sb.Append(char.ToUpper(c));
            }
            return sb.ToString();
        }

        public string GenerateHandlerHeader()
        {
            var sb = new StringBuilder();
            sb.AppendLine("""
            #pragma once
            #include "define.h" 
            class PacketHandler_Auto
            {
            public:
            """);

            foreach (var p in clientPackets_)
            {
                var args = string.Join(", ", p.Fields.Select(f => $"{f.Type} {f.Name}"));
                sb.AppendLine($"\tvirtual void On{p.Case}(uint32_t sessionId, {args}) = 0;");
            }

            sb.AppendLine("};");
            return sb.ToString();
        }

        public string GenerateProcessHeader()
        {
            var sb = new StringBuilder();
            sb.AppendLine("""
            #pragma once
            #include "PacketHandler_Auto.h"

            #include "Util/Singleton.h"
            #include "Contents/ContentsProcess.h"

            class Session;
            class CPacket;

            class PacketProcess_Auto : public Singleton<PacketProcess_Auto>
            {
                friend class Singleton<PacketProcess_Auto>;
            public:
                void Process(uint16_t type, uint32_t sessionId, CPacket& packet);

            private:
                PacketProcess_Auto() : handler(ContentsProcess::Instance()) {}
            """);

            foreach (var p in clientPackets_)
                sb.AppendLine($"\tvoid Dispatch{p.Case}(uint32_t sessionId, CPacket& packet);");

            sb.AppendLine("""
                PacketHandler_Auto& handler;
            };
            """);

            return sb.ToString();
        }

        public string GenerateProcessCpp()
        {
            var sb = new StringBuilder();
            sb.AppendLine("""
            #include "stdafx.h"
            #include "PacketProcess_Auto.h"
            #include "PacketHandler_Auto.h"

            #include "Util/SystemLogger.h"
            #include "Util/CPacket.h"
            #include "NetLib/Session.h"
            #include "Contents/ContentsProcess.h"

            using enum SystemLogger::LOG_LEVEL;

            void PacketProcess_Auto::Process(uint16_t type, uint32_t sessionId, CPacket& packet)
            {
                switch (type) {
            """);

            for (int i = 0; i < clientPackets_.Count; i++)
                sb.AppendLine($"\tcase {i}: Dispatch{clientPackets_[i].Case}(sessionId, packet); break;");

            sb.AppendLine("""
                default:
                    SLog(ERROR_LEVEL, L"Packet Process default case # type is (%d)", type);
                    break;
                }
            }
            """);

            foreach (var p in clientPackets_)
            {
                sb.AppendLine($"void PacketProcess_Auto::Dispatch{p.Case}(uint32_t sessionId, CPacket& packet)");
                sb.AppendLine("{");

                foreach (var f in p.Fields)
                    sb.AppendLine($"\t{f.Type} {f.Name};");

                foreach (var f in p.Fields)
                    sb.AppendLine($"\tpacket >> {f.Name};");

                var argList = string.Join(", ", p.Fields.Select(f => f.Name));
                sb.AppendLine($"\thandler.On{p.Case}(sessionId, {argList});");
                sb.AppendLine("}");
            }

            return sb.ToString();
        }

        public string GenerateMakerHeader()
        {
            var sb = new StringBuilder();
            sb.AppendLine("""
            #pragma once
            #include "Util/CPacket.h"
            """); 

            foreach (var p in serverPackets_)
            {
                string h_line = $"void mp_{p.Case}(CPacket& packet";
                foreach (var field in p.Fields)
                {
                    h_line += $", {field.Type} {field.Name}";
                }
                h_line += $");";
                sb.AppendLine(h_line);
            }

            return sb.ToString();
        }

        public string GenerateMakerCpp()
        {
            var sb = new StringBuilder();
            sb.AppendLine("""
            #include "stdafx.h"
            #include "PacketMaker_Auto.h"
            #include "define.h"

            """);

            int typeIndex = 0;

            foreach (var p in serverPackets_)
            {
                string cpp_func = $"void mp_{p.Case}(CPacket& packet";
                string size_sum_string = string.Empty;
                if (p.Fields.Any())
                {
                    size_sum_string = string.Join(" + ", p.Fields.Select(f => $"sizeof({f.Type})"));
                }
                else
                {
                    size_sum_string = "0";
                }

                foreach (var field in p.Fields)
                {
                    cpp_func += $", {field.Type} {field.Name}";
                }
                cpp_func += ")\n{\n";

                cpp_func += "\tPktHeader pktHeader;\n";

                cpp_func += "\tpktHeader.Code = 0x98; \n";

                cpp_func += $"\tpktHeader.Type = {typeIndex};\n";

                cpp_func += $"\tpktHeader.Len = {size_sum_string};\n";

                cpp_func += "\tpacket.Clear();\n";
                cpp_func += "\tpacket.PutData((char*)&pktHeader, sizeof(PktHeader));\n";

                cpp_func += "\tpacket";
                foreach (var field in p.Fields)
                {
                    cpp_func += $" << {field.Name}";
                }
                cpp_func += ";\n}";

                sb.AppendLine(cpp_func);
                sb.AppendLine();

                typeIndex++;
            }

            return sb.ToString();
        }
    }
}