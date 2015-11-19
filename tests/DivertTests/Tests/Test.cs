/*
* This software is ported from the original test application for WinDivert,
* which can be found at https://github.com/basil00/Divert/blob/master/test/.
* The License for this software remains under the license of the original
* Author. Note however that the tests are under a different license than the
* CLR library.
*
* Program.cs, ported from test_data.c and test.c
* Copyright © 2015 basil (reqrypt.org), all rights reserved
* Copyright © 2015 Jesse Nicholson
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

namespace DivertTests.Tests
{
    internal class Test
    {
        internal Test(string testName, string filterStr, string dataStr, string matchStr)
        {
            Filter = filterStr.Trim();
            Match = matchStr.Trim().Equals("TRUE", System.StringComparison.OrdinalIgnoreCase);
            
            switch(dataStr.Trim())
            {
                case "&pkt_echo_request":
                    TestPacketData = TestData.EchoRequest;
                break;

                case "&pkt_http_request":
                    TestPacketData = TestData.HttpRequest;
                    break;

                case "&pkt_dns_request":
                    TestPacketData = TestData.DnsRequest;
                    break;

                case "&pkt_ipv6_tcp_syn":
                    TestPacketData = TestData.IPv6TCPSyn;
                    break;

                case "&pkt_ipv6_echo_reply":
                    TestPacketData = TestData.IPv6EchoReply;
                    break;

                case "&pkt_ipv6_exthdrs_udp":
                    TestPacketData = TestData.IPv6ExtraHeadersUdp;
                    break;

                default:
                    throw new System.Exception(string.Format("Unknown data specified in test: {0}", testName));
            }
        }

        public string Filter
        {
            get;
            protected set;
        }

        public bool Match
        {
            get;
            protected set;
        }

        public string TestName
        {
            get;
            protected set;
        }

        public byte[] TestPacketData
        {
            get;
            protected set;
        }
    }
}