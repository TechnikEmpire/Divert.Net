using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Divert.Net;

namespace NetDump
{
    class Program
    {
        static void Main(string[] args)
        {
            bool running = true;

            Console.CancelKeyPress += delegate {
                running = false;
            };

            Diversion diversion;

            string filter = "true";

            try
            {                
                diversion = Diversion.Open(filter, DivertLayer.Network, 100, FilterFlags.Sniff);
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                return;
            }

            if(!diversion.Handle.Valid)
            {
                Console.WriteLine("Failed to open divert handle with error {0}", System.Runtime.InteropServices.Marshal.GetLastWin32Error());
                return;
            }

            IPHeader ipHeader = new IPHeader();
            IPv6Header ipv6Header = new IPv6Header();
            ICMPHeader icmpHeader = new ICMPHeader();
            ICMPv6Header icmpv6Header = new ICMPv6Header();
            TCPHeader tcpHeader = new TCPHeader();
            UDPHeader udpHeader = new UDPHeader();

            Address address = new Address();

            byte[] buffer = new byte[65535];

            uint receiveLength = 0;

            while(running)
            {
                receiveLength = 0;

                if (!diversion.Receive(buffer, address, ref receiveLength))
                {
                    Console.WriteLine("Failed to receive packet with error {0}", System.Runtime.InteropServices.Marshal.GetLastWin32Error());
                    continue;
                }

                diversion.ParsePacket(buffer, receiveLength, ipHeader, ipv6Header, icmpHeader, icmpv6Header, tcpHeader, udpHeader);

                if (ipHeader.Valid && tcpHeader.Valid)
                {
                    Console.WriteLine("{0} TCP packet captured destined for {1}:{2} from {3}:{4}", address.Direction == DivertDirection.Inbound ? "Inbound" : "Outbound", ipHeader.DestinationAddress.ToString(), tcpHeader.DestinationPort.ToString(), ipHeader.SourceAddress.ToString(), tcpHeader.SourcePort.ToString());
                }
                else if(ipHeader.Valid && udpHeader.Valid)
                {
                    Console.WriteLine("{0} UDP packet captured destined for {1}:{2} from {3}:{4}", address.Direction == DivertDirection.Inbound ? "Inbound" : "Outbound", ipHeader.DestinationAddress.ToString(), udpHeader.DestinationPort.ToString(), ipHeader.SourceAddress.ToString(), udpHeader.SourcePort.ToString());
                }
            }

            diversion.Close();
        }
    }
}
