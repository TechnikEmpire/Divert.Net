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

            // This is not really like the netdump example in the native WinDivert examples. Since we are 
            // pulling the names of processes behind packet flows, we need to fully intercept/divert packets,
            // make the process query, then reinject. If we just sniff, the process will either be closed or
            // no longer bound to the local port the packet is associated with, and the process query will
            // be hit or miss (probably fail). So, by fully diverting rather than sniffing, we force
            // the process to hang open waiting for the packet while we check the process identity, then
            // hand the packet over untouched.
            //
            // Ideally you do not want to be querying the process on every single packet. Rather, you would
            // create a stucture that keeps track of a network flow, identify the process (protocol, whatever
            // else) one time, then only re-check when the flow has ended and a new flow has begun. This is
            // just for basic demonstration though, so we don't create and track any flows.
            //
            // Note also that the process identification is still not 100%. Many system processes run under
            // PID 4. I'm not satisfied with just getting SYSTEM for these processes, and I'd like to
            // ideally be able to identify exactly which processes they are. Still working on that.

            Console.WindowWidth = Console.LargestWindowWidth;

            bool running = true;

            Console.CancelKeyPress += delegate {
                running = false;
            };

            Diversion diversion;

            string filter = "true";

            try
            {                
                diversion = Diversion.Open(filter, DivertLayer.Network, 100, 0);
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
            uint sendLength = 0;

            string processName;

            uint pid = 0;

            while (running)
            {
                pid = 0;

                receiveLength = 0;
                sendLength = 0;

                if (!diversion.Receive(buffer, address, ref receiveLength))
                {
                    Console.WriteLine("Failed to receive packet with error {0}", System.Runtime.InteropServices.Marshal.GetLastWin32Error());
                    continue;
                }

                diversion.ParsePacket(buffer, receiveLength, ipHeader, ipv6Header, icmpHeader, icmpv6Header, tcpHeader, udpHeader);

                if (ipHeader.Valid && tcpHeader.Valid)
                {                    
                    Diversion.GetPacketProcess(address, tcpHeader, ipHeader, ref pid, out processName);

                    if (processName.Equals("SYSTEM", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.WriteLine("ERROR {0} and PID is {1}", System.Runtime.InteropServices.Marshal.GetLastWin32Error(), pid);
                    }

                    Console.WriteLine(
                        "{0} IPv4 TCP packet captured destined for {1}:{2} from {3}:{4} {5}.", 
                        address.Direction == DivertDirection.Inbound ? "Inbound" : "Outbound", 
                        ipHeader.DestinationAddress.ToString(), tcpHeader.DestinationPort.ToString(), 
                        ipHeader.SourceAddress.ToString(), tcpHeader.SourcePort.ToString(),
                        address.Direction == DivertDirection.Inbound ? string.Format("to process {0}", processName) : string.Format("from process {0}", processName)
                        );
                }
                else if(ipHeader.Valid && udpHeader.Valid)
                {
                    Diversion.GetPacketProcess(address, udpHeader, ipHeader, ref pid, out processName);

                    if (processName.Equals("SYSTEM", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.WriteLine("ERROR {0} and PID is {1}", System.Runtime.InteropServices.Marshal.GetLastWin32Error(), pid);
                    }

                    Console.WriteLine(
                        "{0} IPv4 UDP packet captured destined for {1}:{2} from {3}:{4} {5}.",
                        address.Direction == DivertDirection.Inbound ? "Inbound" : "Outbound",
                        ipHeader.DestinationAddress.ToString(), tcpHeader.DestinationPort.ToString(),
                        ipHeader.SourceAddress.ToString(), tcpHeader.SourcePort.ToString(),
                        address.Direction == DivertDirection.Inbound ? string.Format("to process {0}", processName) : string.Format("from process {0}", processName)
                        );                   
                }
                else if(ipv6Header.Valid && tcpHeader.Valid)
                {
                    Diversion.GetPacketProcess(address, tcpHeader, ipv6Header, ref pid, out processName);

                    if (processName.Equals("SYSTEM", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.WriteLine("ERROR {0} and PID is {1}", System.Runtime.InteropServices.Marshal.GetLastWin32Error(), pid);
                    }

                    Console.WriteLine(
                        "{0} IPv6 TCP packet captured destined for {1}:{2} from {3}:{4} {5}.",
                        address.Direction == DivertDirection.Inbound ? "Inbound" : "Outbound",
                        ipHeader.DestinationAddress.ToString(), tcpHeader.DestinationPort.ToString(),
                        ipHeader.SourceAddress.ToString(), tcpHeader.SourcePort.ToString(),
                        address.Direction == DivertDirection.Inbound ? string.Format("to process {0}", processName) : string.Format("from process {0}", processName)
                        );                    
                }
                else if (ipv6Header.Valid && udpHeader.Valid)
                {
                    Diversion.GetPacketProcess(address, udpHeader, ipv6Header, ref pid, out processName);

                    if (processName.Equals("SYSTEM", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.WriteLine("ERROR {0} and PID is {1}", System.Runtime.InteropServices.Marshal.GetLastWin32Error(), pid);
                    }

                    Console.WriteLine(
                        "{0} IPv6 UDP packet captured destined for {1}:{2} from {3}:{4} {5}.",
                        address.Direction == DivertDirection.Inbound ? "Inbound" : "Outbound",
                        ipHeader.DestinationAddress.ToString(), tcpHeader.DestinationPort.ToString(),
                        ipHeader.SourceAddress.ToString(), tcpHeader.SourcePort.ToString(),
                        address.Direction == DivertDirection.Inbound ? string.Format("to process {0}", processName) : string.Format("from process {0}", processName)
                        );                   
                }

                if(address.Direction == DivertDirection.Outbound)
                {
                    diversion.CalculateChecksums(buffer, receiveLength, 0);
                }

                diversion.SendAsync(buffer, receiveLength, address, ref sendLength);
            }

            diversion.Close();
        }
    }
}
